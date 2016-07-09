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

void setRetardo(int newRetardo){
	retardo = newRetardo;
}

void dumpAllTables(){
	;
}

void dumpTable(int pid){
	tabla_de_paginas* tabla = buscarPorPID(pid);
	if(tabla == NULL){
		printf("Fallo: tabla no encontrada");
	}
	printPagesTable(tabla);
}

void dumpAllData(){
	;
}

void dumpData(int pid){
	;
}

void flushTlb(){
	;
}

void flushMemory(int pid){
	;
}

