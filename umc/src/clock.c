#include "clock.h"

#include "umc-structs.h"

int agujaClock; //nroFrame apuntado actualmente

void initClock(){
	agujaClock = 0;
}

response* clockGetFrame(int nroPagina, int pidActivo){
	//buscar en Tabla de Paginas del pid
	tabla_de_paginas_entry* entrada = buscarPorNroPaginaYPID(nroPagina, pidActivo);
	if(entrada==NULL){
		return createFAILResponse(PAGINA_NO_EXISTE);
	}

	if(entrada->presente){ //aguja sigue igual y bit de referencia del frame se pone en 1
		tabla_de_frame_entry* entradaFrame = obtenerEntradaDeFrame(entrada->nroFrame);
		entradaFrame->referenciado = 1;
		entrada->modificado = 1;
		return createResponse(1,0,sizeof(int32_t),entrada->nroFrame);
	}

	//Hay page fault
	tabla_de_frame_entry* victima;
	int victimaEncontrada = 0;
	while(!victimaEncontrada){
		victima = obtenerEntradaDeFrame(agujaClock);
		if(victima->referenciado){ //no lo uso de victima, pero le pongo el bit r en 0, y muevo la aguja
			victima->referenciado = 0;
		} else{
			victimaEncontrada = 1; // para el ciclo while
		}
		agujaClock++; //en ambos casos el puntero se mueve
	}

	// A esta altura, ya se que hay page fault, y ya se que frame reemplazar

	//PERO: antes de reemplazar, si el frame tiene bit modificado ⇒ actualizar en swap
	int pidVictima = buscarPIDActualDeFrame(victima->nroFrame);
	int nroPaginaVictima = buscarNroPagActualDeFrame(victima->nroFrame);
	tabla_de_paginas_entry* entradaPaginaVictima = buscarPorNroPaginaYPID(nroPaginaVictima, pidVictima);
	if(entradaPaginaVictima->modificado){
		escribirPaginaEnSwap(nroPagina, pidVictima, victima->direccion_real);
	}

	return createResponse(1,0,sizeof(int32_t),victima->nroFrame);
}
