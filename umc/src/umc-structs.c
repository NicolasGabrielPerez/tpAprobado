#include "umc-structs.h"
#include "clock.h"

int cantidad_de_marcos;
int marco_size;
int marcos_x_proc;
char* memoria_bloque;
tabla_de_frames* tablaDeFrames;
t_list* tablasDePaginas;
int algoritmoClockEnable;
int algoritmoClockModificadoEnable;

int initMemoriaPrincipal(t_config* config){
	cantidad_de_marcos = config_get_int_value(config, "MARCOS");
	marco_size = config_get_int_value(config, "MARCO_SIZE");
	marcos_x_proc = config_get_int_value(config, "MARCOS_X_PROC");

	algoritmoClockEnable = config_get_int_value(config, "CLOCK");
	algoritmoClockModificadoEnable = config_get_int_value(config, "CLOCK");

	if(algoritmoClockEnable) agujaClock = 0;
	if(algoritmoClockModificadoEnable)

	memoria_bloque = malloc(cantidad_de_marcos*marco_size); //char* que va a tener el contenido de todas las paginas

	tablaDeFrames = malloc(sizeof(tablaDeFrames));
	tablaDeFrames->entradas = list_create();
	int i;
	for(i=0; i<cantidad_de_marcos; i++) {
		tabla_de_frame_entry* entrada = malloc(sizeof(tabla_de_frame_entry));
		entrada->nroFrame = i;
		entrada->ocupado = 0;
		entrada->referenciado = 0;
		entrada->direccion_real = &memoria_bloque[i*marco_size];
		list_add(tablaDeFrames->entradas, entrada);
		free(entrada);
	}

	tablasDePaginas = list_create();

	return EXIT_SUCCESS;
}

tabla_de_paginas* buscarPorPID(int pidABuscar){
	int i;
	for(i=0; i<list_size(tablasDePaginas);i++){
		tabla_de_paginas* actual = list_get(tablasDePaginas, i);
		printf("pid actual:%d\n", actual->pid);
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
		printf("nroPagina actual:%d\n", actual->nroPagina);
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

char* initProgramaUMC(int pid, int cantPaginas){
	// verificar que no exista pid
	if(buscarPorPID(pid)!=NULL){
		return string_itoa(RESPUESTA_FAIL);
	}
	// verificar que cantPaginas + stackSize < MAX
	if(cantPaginas > marcos_x_proc){
		return string_itoa(RESPUESTA_FAIL);
	}
	// crear tabla de paginas
	tabla_de_paginas* tablaDePaginas = malloc(sizeof(tabla_de_paginas));
	tablaDePaginas->entradas = list_create();
	tablaDePaginas->pid = pid;

	list_add(tablasDePaginas, tablaDePaginas);
	return string_itoa(RESPUESTA_OK);
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

void escribirEnFrame(char* buffer, int offset, int tamanio, int frame){
	tabla_de_frame_entry* entrada = obtenerEntradaDeFrame(frame);
	memcpy(entrada->direccion_real + offset, buffer, tamanio);
}

char* obtenerBytesDeMemoriaPrincipal(int frame, int offset, int tamanio){
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
		puts("Error: memoria llena");
		return;
	}
	cargarEnMemoriaPrincipal(pagina, nroFrameACargar);
	entrada->nroFrame = nroFrameACargar;
	entrada->presente = 1;
	return;
}
