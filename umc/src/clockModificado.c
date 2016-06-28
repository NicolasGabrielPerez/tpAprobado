#include "clockModificado.h"

#include "umc-structs.h"

int doFirstFetch(tabla_de_paginas* tabla){
	int aguja = tabla->punteroClockModificado;

	tabla_de_paginas_entry* entrada = list_get(tabla.entradas, aguja);
	if(!entrada->modificado && !entrada->uso){
		tabla->punteroClockModificado ++;
		return aguja;
	}

	int indice;

	//Recorrer a partir de la siguiente entrada,
	// Hasta que, llegado el caso, se vuelva a la misma
	for(indice = aguja + 1; indice != aguja; indice++){
		if(indice == marcos_x_proc){
			indice = 0; //dar la vuelta (empezar desde el principio de la tabla)
			continue;
		}

		tabla->punteroClockModificado = indice;

		entrada = list_get(tabla.entradas, indice);
		if(!entrada->modificado && !entrada->uso){
			return indice;
		}
	}

	return -1;
}

int doSecondFetch(tabla_de_paginas* tabla){
	int aguja = tabla->punteroClockModificado;

	tabla_de_paginas_entry* entrada = list_get(tabla.entradas, aguja);
	if(entrada->modificado && !entrada->uso){
		tabla->punteroClockModificado ++;
		return aguja;
	} else{
		entrada->uso = 0;
	}

	int indice;

	//Recorrer a partir de la siguiente entrada,
	// Hasta que, llegado el caso, se vuelva a la misma
	for(indice = aguja + 1; indice != aguja; indice++){
		if(indice == marcos_x_proc){
			indice = 0; //dar la vuelta (empezar desde el principio de la tabla)
			continue;
		}

		tabla->punteroClockModificado = indice;

		entrada = list_get(tabla.entradas, indice);
		if(entrada->modificado && !entrada->uso){
			return indice;
		} else{
			entrada->uso = 0;
		}
	}

	return -1;

}

//Buscar victima a reemplazar
//No contemplo errores
//Asumo que hay page fault
tabla_de_paginas_entry* clockModificadoGetVictima(tabla_de_paginas* tablaDePaginas){

	int nroPaginaVictima = -1;

	while(nroPaginaVictima == -1){
		nroPaginaVictima = doFirstFetch(tablaDePaginas);

		if(nroPaginaVictima == -1){
			nroPaginaVictima = doSecondFetch(tablaDePaginas);
		}
	}

	return list_get(tablaDePaginas->entradas, nroPaginaVictima);
}
