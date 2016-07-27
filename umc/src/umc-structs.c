#include "umc-structs.h"

#include "pages_replacement.h"
#include <time.h>

int cantidad_de_marcos;
int marco_size;
int marcos_x_proc;
int retardo;

char* memoria_bloque;
tabla_de_frames* tablaDeFrames;
t_list* tablasDePaginas;
enum AlgoritmoReemplazo algoritmoActivo;

t_log* logger;

void demorarSolicitud()
{
   struct timespec req, rem;

   if(retardo > 999)
   {
        req.tv_sec = (int)(retardo / 1000);                            /* Must be Non-Negative */
        req.tv_nsec = (retardo - ((long)req.tv_sec * 1000)) * 1000000; /* Must be in range of 0 to 999999999 */
   }
   else
   {
        req.tv_sec = 0;                         /* Must be Non-Negative */
        req.tv_nsec = retardo * 1000000;    /* Must be in range of 0 to 999999999 */
   }

   nanosleep(&req , &rem);
}

int initMemoriaPrincipal(t_config* config){
	cantidad_de_marcos = config_get_int_value(config, "MARCOS");
	marco_size = config_get_int_value(config, "MARCO_SIZE");
	marcos_x_proc = config_get_int_value(config, "MARCOS_X_PROC");
	retardo = config_get_int_value(config, "RETARDO");
	char* algoritmo = config_get_string_value(config, "ALGORITMO");

	if(string_equals_ignore_case(algoritmo, "clock")){
		algoritmoActivo = clockComun;
	} else{
		algoritmoActivo = clockModificado;
	}

	memoria_bloque = malloc(cantidad_de_marcos*marco_size); //char* que va a tener el contenido de todas las paginas

	log_info(logger, "Iniciando memoria principal..........................");
	log_info(logger, "Cantidad de marcos: %d | Tamanio de marco: %d", cantidad_de_marcos, marco_size);
	log_info(logger, "Reservada memoria. Total de bytes:%d", cantidad_de_marcos*marco_size);

	tablaDeFrames = malloc(sizeof(tablaDeFrames));
	tablaDeFrames->entradas = list_create();
	int i;
	for(i=0; i<cantidad_de_marcos; i++) {
		tabla_de_frame_entry* entrada = malloc(sizeof(tabla_de_frame_entry));
		entrada->nroFrame = i;
		entrada->ocupado = 0;
		entrada->direccion_real = &memoria_bloque[i*marco_size];
		list_add(tablaDeFrames->entradas, entrada);
	}

	log_trace(logger, "Creada tabla de frames! Cantidad de entradas:%d", list_size(tablaDeFrames->entradas));

	tablasDePaginas = list_create();

	return EXIT_SUCCESS;
}

tabla_de_paginas* buscarPorPID(int pidABuscar){
	int i;
	for(i=0; i<list_size(tablasDePaginas);i++){
		tabla_de_paginas* actual = list_get(tablasDePaginas, i);
		if(actual->pid == pidABuscar){
			return actual;
		}
	}
	return NULL;
};

tabla_de_paginas_entry* buscarEntradaPorPagina(tabla_de_paginas* tablaDePaginas, int nroPagina){
	int i;
	for(i=0; i<list_size(tablaDePaginas->entradas);i++){
		tabla_de_paginas_entry* actual = list_get(tablaDePaginas->entradas, i);
		if(actual->nroPagina == nroPagina){
			return actual;
		}
	}
	return NULL;
};

tabla_de_paginas_entry* buscarPorNroPaginaYPID(int nroPagina, int pid){
	tabla_de_paginas* tablaDePaginas = buscarPorPID(pid);
	return buscarEntradaPorPagina(tablaDePaginas, nroPagina);
}

tabla_de_paginas* crearTablaDePaginas(int pid, int cantPaginas){
	tabla_de_paginas* tablaDePaginas = malloc(sizeof(tabla_de_paginas));
	tablaDePaginas->entradas = list_create();
	tablaDePaginas->pid = pid;

	int i;
	for(i=0; i<cantPaginas; i++){
		tabla_de_paginas_entry* pageEntry = malloc(sizeof(tabla_de_paginas_entry));
		pageEntry->nroPagina = i;
		pageEntry->pid = pid;
		pageEntry->presente = 0;
		pageEntry->uso = 0;
		pageEntry->modificado = 0;
		list_add(tablaDePaginas->entradas, pageEntry);
	}
	return tablaDePaginas;
}

void* destroyPageEntry(void* element){
	tabla_de_paginas_entry* pageEntry = element;
	free(pageEntry);
	return 0;
}

void* destroyPresente(void* element){
	presente* presente = element;
	free(presente);
	return 0;
}

void* destroyPagesTable(void* element){
	tabla_de_paginas* tabla = element;
	list_destroy_and_destroy_elements(tabla->entradas, destroyPageEntry);
	tabla->entradas = NULL;
	list_destroy_and_destroy_elements(tabla->presentes, destroyPresente);
	tabla->presentes = NULL;
	free(tabla);
	tabla = NULL;
	return 0;
}

int getPagesTableIndex(t_list* list, int pid){
	int i;
	tabla_de_paginas* table;
	for(i=0; i<list_size(list); i++){
		table = list_get(list, i);
		if(table->pid == pid) return i;
	}

	return -1;
}

void borrarTablaDePaginas(tabla_de_paginas* tablaDePaginas){
	int pagesTableIndex = getPagesTableIndex(tablasDePaginas, tablaDePaginas->pid);

	if(pagesTableIndex==-1){
		log_error(logger, "Error al borrar tabla de paginas. Tabla no encontrada");
		printf("Tabla no encontrada\n");
		return;
	}

	list_remove_and_destroy_element(tablasDePaginas, pagesTableIndex, destroyPagesTable);
}

response* finalizarPidDeUMC(int pid){

	demorarSolicitud();

	tabla_de_paginas* tablaDePaginas = buscarPorPID(pid);

	// verificar que exista pid
	if(tablaDePaginas==NULL){
		return createFAILResponse(PID_NO_EXISTE);
	}

	borrarTablaDePaginas(tablaDePaginas);

	return createOKResponse();
}

response* initProgramaUMC(int pid, int cantPaginas){
	// verificar que no exista pid
	if(buscarPorPID(pid)!=NULL){
		return createFAILResponse(RESPUESTA_FAIL);
	}

	demorarSolicitud();
	// crear tabla de paginas
	tabla_de_paginas* tablaDePaginas = crearTablaDePaginas(pid, cantPaginas);
	log_trace(logger, "Create tabla de paginas de pid %d!", pid);
	t_list* presentes = list_create();

	int i;
	for(i = 0; i<marcos_x_proc; i++){
		presente* presenteNuevo = malloc(sizeof(presente));
		presenteNuevo->nroFrame = -1;
		presenteNuevo->nroPagina = -1;
		presenteNuevo->uso = 0;
		presenteNuevo->modificado = 0;
		list_add(presentes, presenteNuevo);
	}

	tablaDePaginas->presentes = presentes;

	tablaDePaginas->aguja = 0;

	list_add(tablasDePaginas, tablaDePaginas);
	return createOKResponse();
}

char* obtenerBytes(char* contenido, int offset, int tamanio){
	char* bytes = malloc(tamanio);
	memcpy(bytes, contenido+offset, tamanio);
	return bytes;
}

tabla_de_frame_entry* obtenerEntradaDeFrame(int nroFrame){
	int i;
	for(i=0; i<list_size(tablaDeFrames->entradas);i++){
		tabla_de_frame_entry* actual = list_get(tablaDeFrames->entradas, i);
		if(actual->nroFrame == nroFrame){
			return actual;
		}
	}
	return NULL;
}

void escribirEnFrame(char* buffer, int offset, int tamanio, int nroFrame){
	tabla_de_frame_entry* entrada = obtenerEntradaDeFrame(nroFrame);
    char* where = entrada->direccion_real + offset;
	memcpy(where, buffer, tamanio);
	log_trace(logger, "Frame %d escrito!", nroFrame);
}

char* obtenerBytesDeMemoriaPrincipal(int frame, int offset, int tamanio){
	demorarSolicitud();
	int i;
	for(i=0; i<list_size(tablaDeFrames->entradas);i++){
		tabla_de_frame_entry* actual = list_get(tablaDeFrames->entradas, i);
		if(actual->nroFrame == frame){
			return obtenerBytes(actual->direccion_real, offset, tamanio);
		}
	}
	return NULL;
}

bool paginaNoPresente(void* entrada){
	tabla_de_paginas_entry* entradaDePagina = (tabla_de_paginas_entry*) entrada;
	return !entradaDePagina->presente;
}

tabla_de_paginas_entry* obtenerEntradaDePagina(int pid, int nroPagina){
	tabla_de_paginas* tablaDePaginas = buscarPorPID(pid);
	tabla_de_paginas_entry* entrada = buscarEntradaPorPagina(tablaDePaginas, nroPagina);
	return entrada;
}

int obtenerFrameDisponible(){
	int i;
	for(i=0; i<list_size(tablaDeFrames->entradas);i++){
		tabla_de_frame_entry* actual = list_get(tablaDeFrames->entradas, i);
		if(!actual->ocupado){
			return actual->nroFrame;
		}
	};
	return -1;
}

void cargarEnMemoriaPrincipal(char* pagina, int nroFrame){
	tabla_de_frame_entry* entrada = obtenerEntradaDeFrame(nroFrame);
	memcpy(entrada->direccion_real, pagina, marco_size);
}

void cargarPagina(int nroPagina, int pid, char* pagina){
	tabla_de_paginas_entry* entrada = obtenerEntradaDePagina(pid, nroPagina);
	int nroFrameACargar = obtenerFrameDisponible();
	if(nroFrameACargar == -1){
		printf("Error: memoria llena\n");
		return;
	}
	cargarEnMemoriaPrincipal(pagina, nroFrameACargar);
	entrada->nroFrame = nroFrameACargar;
	entrada->presente = 1;
	return;
}

char* leerFrame(int nroFrame){
	return obtenerBytesDeMemoriaPrincipal(nroFrame, 0, marco_size);
}

int tablaDePaginasIsFromFrame(tabla_de_paginas* tablaDePaginas, int nroFrame){
	int i;
	for(i=0; i<list_size(tablaDePaginas->entradas); i++){
		tabla_de_paginas_entry* paginaEntry = list_get(tablaDePaginas->entradas, i);
		if(paginaEntry->nroFrame == nroFrame) return 1;
	}
	return 0;
}

int buscarPIDActualDeFrame(int nroFrame){
	int i;
	for(i=0; i<list_size(tablasDePaginas); i++){
		tabla_de_paginas* tablaDePaginas = list_get(tablasDePaginas, i);
		if(tablaDePaginasIsFromFrame(tablaDePaginas, nroFrame)){
			return tablaDePaginas->pid;
		}
	}
	return 0;
}

int buscarNroPagActualDeFrame(int nroFrame){
	int i;
	for(i=0; i<list_size(tablasDePaginas); i++){
		tabla_de_paginas* tablaDePaginas = list_get(tablasDePaginas, i);

		int j;
		for(j=0; j<list_size(tablaDePaginas->entradas);i++){
			tabla_de_paginas_entry* paginaEntry = list_get(tablaDePaginas->entradas, j);
			if(paginaEntry->nroFrame == nroFrame) return paginaEntry->nroPagina;
		}
	}
	return 0;
}

tabla_de_frame_entry* getFrameEntry(int nroFrame){
	int i;
	tabla_de_frame_entry* tablaDeFramesEntry;
	for(i=0; i<list_size(tablaDeFrames->entradas); i++){
		tablaDeFramesEntry = list_get(tablaDeFrames->entradas, i);
		if(tablaDeFramesEntry->nroFrame == nroFrame){
			break;
		}
	}
	return tablaDeFramesEntry;
}

umcResult createUmcResult(int ok, int codError, tabla_de_paginas_entry* paginaEntry, tabla_de_frame_entry* frameEntry){
	umcResult result;
	result.ok = ok;
	result.codError = codError;
	result.paginaEntry = paginaEntry;
	result.frameEntry = frameEntry;
	return result;
};

umcResult createFAILResult(int codError){
	return createUmcResult(0, codError, 0, 0);
}

umcResult createOkPageResult(tabla_de_paginas_entry* paginaEntry){
	return createUmcResult(1, 0, paginaEntry, 0);
}

umcResult createOkFrameResult(tabla_de_frame_entry* frameEntry){
	return createUmcResult(1, 0, 0, frameEntry);
}

void initLogger(){
	logger = log_create("umc.log", "UMC",true, LOG_LEVEL_TRACE);
	log_trace(logger, "---------------INIT LOG----------------");
}

