#include "pages_replacement.h"
#include "swap-interfaz.h"
#include "umc-structs.h"

enum AlgoritmoReemplazo algoritmo;

pthread_mutex_t search_free_frame = PTHREAD_MUTEX_INITIALIZER;

void init(enum AlgoritmoReemplazo algoritmoReemplzao){
	algoritmo = algoritmoReemplzao;
}

void cargarNroPagYFrame(presente* presente, tabla_de_paginas* tablaDePaginas, int nroPagina){
	if((presente->nroPagina != -1) && presente->modificado){ //REEMPLAZO DE PAGINA
		//Escribir en swap lo que va a ser reemplazado
		tabla_de_frame_entry* victimaFrame = obtenerEntradaDeFrame(presente->nroFrame);
		escribirPaginaEnSwap(presente->nroPagina, tablaDePaginas->pid, victimaFrame->direccion_real);
		//en este caso, presente ya viene con nroFrame
	}

	if(presente->nroPagina == -1){ //Todavia hay lugar libre
		pthread_mutex_lock(&search_free_frame);
			int nroFrameDisponible = obtenerFrameDisponible();
			tabla_de_frame_entry* frameDisponible = obtenerEntradaDeFrame(nroFrameDisponible);
			frameDisponible->ocupado = 1;
		pthread_mutex_unlock(&search_free_frame);

		presente->nroFrame = nroFrameDisponible;
	}
	presente->nroPagina = nroPagina;
}

void cargarEnPresentes(int pid, presente* presente){
	response* response = pedirPaginaASwap(presente->nroPagina, pid);

	if(!response->ok){
		return;
	}

	cargarEnMemoriaPrincipal(response->contenido, presente->nroFrame);
}

presente* siguientePresente(tabla_de_paginas* tablaDePaginas){
	t_list* presentes = tablaDePaginas->presentes;

	tablaDePaginas->aguja++;

	if(tablaDePaginas->aguja == marcos_x_proc){
		tablaDePaginas->aguja = 0;
	}

	return list_get(presentes, tablaDePaginas->aguja);
}

presente* clockModificado_Paso1(tabla_de_paginas* tablaDePaginas){
	presente* actual = list_get(tablaDePaginas->presentes, tablaDePaginas->aguja);

	int agujaOriginal = tablaDePaginas->aguja;

	while(1){

		if(!actual->uso && !actual->modificado){
			return actual;
		}

		actual = siguientePresente(tablaDePaginas);

		if(tablaDePaginas->aguja == agujaOriginal)return NULL; //se cumplio un ciclo
	}
}

presente* clockModificado_Paso2(tabla_de_paginas* tablaDePaginas){
	presente* actual = list_get(tablaDePaginas->presentes, tablaDePaginas->aguja);

	int agujaOriginal = tablaDePaginas->aguja;

	while(1){

		if(!actual->uso && actual->modificado){
			return actual;
		}

		actual->uso = 0;

		actual = siguientePresente(tablaDePaginas);

		if(tablaDePaginas->aguja == agujaOriginal)return NULL; //se cumplio un ciclo
	}
}

presente* obtenerSegunClockModificado(tabla_de_paginas* tablaDePaginas){

	presente* victima = clockModificado_Paso1(tablaDePaginas);

	if(victima != NULL){
		return victima;
	}

	victima = clockModificado_Paso2(tablaDePaginas);

	if(victima != NULL){
		return victima;
	}

	return obtenerSegunClockModificado(tablaDePaginas);
}

presente* obtenerSegunClock(tabla_de_paginas* tablaDePaginas){
	presente* actual = list_get(tablaDePaginas->presentes, tablaDePaginas->aguja);

	while(actual){ //forever hasta enconctrar con bit u=0

		if(!actual->uso){
			return actual;
		}

		actual->uso = 0;

		actual = siguientePresente(tablaDePaginas);
	}

	return 0; //este return no deberia ocurrir nunca
}

presente* obtenerIndiceDondeCargar(tabla_de_paginas* tablaDePaginas){

	if(algoritmo == clockComun){
		return obtenerSegunClock(tablaDePaginas);
	}

	if(algoritmo == clockModificado){
		return obtenerSegunClockModificado(tablaDePaginas);
	}

	return 0;
}

presente* buscarPresente(tabla_de_paginas* tablaDePaginas, int nroPagina){
	int i;
	presente* presente;
	for(i=0; i<list_size(tablaDePaginas->presentes); i++){
		presente = list_get(tablaDePaginas->presentes, i);
		if(presente->nroPagina == nroPagina){
			break;
		}
	}
	return presente;
}

void actualizarPaginaVictima(int pid, int nroPagina){
	if(nroPagina == -1){
		return; // nada que actualizar
	}
	tabla_de_paginas_entry* victimPage = obtenerEntradaDePagina(pid, nroPagina);
	victimPage->presente = 0;
}

umcResult getPageEntry(tabla_de_paginas* tablaDePaginas, int nroPagina){

	//chequear nroPagina existe
	tabla_de_paginas_entry* pageEntry = buscarPorNroPaginaYPID(nroPagina, tablaDePaginas->pid);
	if(pageEntry==NULL){
		return createFAILResult(PAGINA_NO_EXISTE);
	}

	if(pageEntry->presente){ //NO HAY PAGE FAULT. Aguja sigue igual y bit de referencia de LA PAGINA se pone en 1
		pageEntry->uso = 1;
		presente* presente = buscarPresente(tablaDePaginas, nroPagina);
		presente->uso = 1;
		tabla_de_frame_entry* frameEntry = obtenerEntradaDeFrame(presente->nroFrame);
		return createUmcResult(1, 0, pageEntry, frameEntry);
	}

	//Hay PAGE FAULT
	//Es decir, hay que buscar el indice de la victima, dentro de la lista de presentes
	//Actualizar aguja, y bits de uso
	presente* presenteACargar = obtenerIndiceDondeCargar(tablaDePaginas);

	actualizarPaginaVictima(tablaDePaginas->pid, presenteACargar->nroPagina);

	cargarNroPagYFrame(presenteACargar, tablaDePaginas, nroPagina);

	cargarEnPresentes(tablaDePaginas->pid, presenteACargar); //cargar nuevo presente en memoria real
	pageEntry->presente = 1;
	pageEntry->nroFrame = presenteACargar->nroFrame;
	presenteACargar->uso = 1;
	tabla_de_frame_entry* frameCargado = obtenerEntradaDeFrame(presenteACargar->nroFrame);

	demorarSolicitud();
	return createUmcResult(1, 0, pageEntry, frameCargado);
}
