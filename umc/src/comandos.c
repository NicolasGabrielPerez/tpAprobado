#include "comandos.h"

void printPagesTable(tabla_de_paginas* tabla){
	printf("Tabla de paginas de PID %d\n", tabla->pid);
	int i;
	tabla_de_paginas_entry* entry;
	for(i = 0; i< list_size(tabla->entradas); i++){
		entry = list_get(tabla->entradas, i);
		printf("Pagina: %d | P=%d | Frame= %d | U=%d | M=%d\n",
				entry->nroPagina, entry->presente, entry->nroFrame, entry->uso, entry->modificado);
	}
	printf("----------------- Fin de tabla -----------------\n");
}

void printFrame(char* contenido){
	char* stringFrame = malloc(marco_size);
	memcpy(stringFrame, contenido, marco_size);
	stringFrame[marco_size] = '\0';

	printf("Contenido de frame:\n");

	int caracteresPorLinea = 50;
	int contadorCaracteres = 0;
	while(contadorCaracteres <= marco_size){
		printf("%s\n", string_substring(stringFrame, contadorCaracteres, caracteresPorLinea));
		contadorCaracteres += caracteresPorLinea;
	}
}

void printData(tabla_de_paginas* tabla){
	printf("Datos almacenados de PID %d\n", tabla->pid);
	int i;
	tabla_de_paginas_entry* entry;
	for(i = 0; i< list_size(tabla->entradas); i++){
		entry = list_get(tabla->entradas, i);
		if(entry->presente){
			printf("Pagina: %d | Frame= %d | U=%d | M=%d\n",
							entry->nroPagina, entry->nroFrame, entry->uso, entry->modificado);

			tabla_de_frame_entry* frameEntry = obtenerEntradaDeFrame(entry->nroFrame);
			char* frame = leerFrame(frameEntry->nroFrame);
			printFrame(frame);
		}
	}
	printf("----------------- Fin de contenido -----------------\n");
}

void setRetardo(int newRetardo){
	retardo = newRetardo;
}

void dumpAllTables(){
	int i;
	tabla_de_paginas* tablaActual;
	for(i = 0; i<list_size(tablasDePaginas); i++){
		tablaActual = list_get(tablasDePaginas, i);
		printPagesTable(tablaActual);
	}
}

void dumpTable(int pid){
	tabla_de_paginas* tabla = buscarPorPID(pid);
	if(tabla == NULL){
		printf("Fallo: tabla no encontrada");
	}
	printPagesTable(tabla);
}

void dumpAllData(){
	int i;
	tabla_de_paginas* tablaActual;
	for(i = 0; i<list_size(tablasDePaginas); i++){
		tablaActual = list_get(tablasDePaginas, i);
		printData(tablaActual);
	}
}

void dumpData(int pid){
	tabla_de_paginas* tabla = buscarPorPID(pid);
	if(tabla == NULL){
		printf("Fallo: tabla no encontrada");
	}
	printData(tabla);
}

void flushTlb(){
	;
}

void flushMemory(int pid){
	;
}

