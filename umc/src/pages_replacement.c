#include "pages_replacement.h"
#include "swap-interfaz.h"
#include "umc-structs.h"

enum AlgoritmoReemplazo algoritmo;

void init(enum AlgoritmoReemplazo algoritmoReemplzao){
	algoritmo = algoritmoReemplzao;
}

void prepararPresenteParaSerCargado(presente* presente, tabla_de_paginas* tablaDePaginas, int nroPagina){
	if(presente->nroPagina != -1 && presente->modificado){ //REEMPLAZO DE PAGINA
		//Escribir en swap lo que va a ser reemplazado
		tabla_de_frame_entry* victimaFrame = obtenerEntradaDeFrame(presente->nroFrame);
		escribirPaginaEnSwap(presente->nroPagina, tablaDePaginas->pid, victimaFrame->direccion_real);
		//en este caso, presente ya viene con nroFrame
	}

	if(presente->nroPagina == -1){ //Todavia hay lugar libre
		// ----semaphore starts -----
		int nroFrameDisponible = obtenerFrameDisponible();
		tabla_de_frame_entry* frameDisponible = obtenerEntradaDeFrame(nroFrameDisponible);
		frameDisponible->ocupado = 1;
		// ----semaphore ends -----
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

presente* obtenerSegunClockModificado(tabla_de_paginas* tablaDePaginas){
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

umcResult getPageEntry(tabla_de_paginas* tablaDePaginas, int nroPagina){

	//chequear nroPagina existe
	tabla_de_paginas_entry* pageEntry = buscarPorNroPaginaYPID(nroPagina, tablaDePaginas->pid);
	if(pageEntry==NULL){
		return createFAILResult(PAGINA_NO_EXISTE);
	}

	if(pageEntry->presente){ //NO HAY PAGE FAULT. Aguja sigue igual y bit de referencia de LA PAGINA se pone en 1
		pageEntry->uso = 1;
		presente* presente = buscarPresente(tablaDePaginas, nroPagina);
		tabla_de_frame_entry* frameEntry = obtenerEntradaDeFrame(presente->nroFrame);
		return createUmcResult(1, 0, pageEntry, frameEntry);
	}

	//Hay PAGE FAULT
	//Es decir, hay que buscar el indice de la victima, dentro de la lista de presentes
	//Actualizar aguja, y bits de uso
	presente* presenteACargar = obtenerIndiceDondeCargar(tablaDePaginas);
	prepararPresenteParaSerCargado(presenteACargar, tablaDePaginas, nroPagina);

	cargarEnPresentes(tablaDePaginas->pid, presente); //cargar nuevo presente en memoria real
	tabla_de_frame_entry* frameCargado = obtenerEntradaDeFrame(presenteACargar->nroFrame);

	return createUmcResult(1, 0, pageEntry, frameCargado);
}
