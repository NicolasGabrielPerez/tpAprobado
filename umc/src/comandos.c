#include "comandos.h"

void printPagesTable(tabla_de_paginas* tabla){
	printf("----------------- Comienzo de tabla -----------------\n");
	printf("Tabla de paginas de PID %d\n", tabla->pid);
	int i;
	tabla_de_paginas_entry* entry;
	for(i = 0; i< list_size(tabla->entradas); i++){
		entry = list_get(tabla->entradas, i);
		printf("Pagina: %d | P=%d | Frame= %d | U=%d | M=%d\n",
				entry->nroPagina, entry->presente, entry->nroFrame, entry->uso, entry->modificado);
		printf("------- Fin entrada de tabla -------\n");
	}
	printf("----------------- Fin de tabla -----------------\n");
}

void printFrame(char* contenido){
	printf("%s\n", contenido);
}

void printData(tabla_de_paginas* tabla){
	printf("----------------- Comienzo de contenido de pid %d -----------------\n", tabla->pid);
	printf("Datos almacenados de PID %d\n", tabla->pid);
	int i;
	tabla_de_paginas_entry* entry;
	for(i = 0; i< list_size(tabla->entradas); i++){
		entry = list_get(tabla->entradas, i);
		if(entry->presente){
			printf("Pagina: %d | Frame= %d | U=%d | M=%d\n",
							entry->nroPagina, entry->nroFrame, entry->uso, entry->modificado);

			if(entry->presente){
				char* frame = leerFrame(entry->nroFrame);
				printFrame(frame);
			}

		}
	}
	printf("----------------- Fin de contenido -----------------\n");
}

void setRetardo(int newRetardo){
	retardo = newRetardo;
	log_info(logger, "Seteado nuevo retardo:%d", retardo);
}

void dumpAllTables(){
	printf("Dump all tables\n");
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
	if(!TLBEnable){
		printf("La TLB no esta habilitada\n");
		return;
	}

	list_clean(TLB->entradas);
	log_info(logger, "[TLB] Flush done. Cantidad de entradas actual: %d", sizeof(TLB->entradas));
}

void flushMemory(int pid){
	tabla_de_paginas* tabla = buscarPorPID(pid);
	if(tabla == NULL){
		printf("Fallo: tabla no encontrada");
	};

	int i;
	tabla_de_paginas_entry* entry;
	for(i=0; i<tabla->entradas->elements_count; i++){
		entry = list_get(tabla, i);
		entry->modificado = 0;
	}
	log_info(logger, "Flush memory done");
}

