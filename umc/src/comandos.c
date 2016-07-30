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

	int j;
	printf("Presentes\n");
	for(j=0; j< list_size(tabla->presentes); j++){
		presente* presente = list_get(tabla->presentes, j);
		printf("Pagina: %d | Frame= %d | U=%d | M=%d\n",
						presente->nroPagina, presente->nroFrame, presente->uso, presente->modificado);

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
				frame[marco_size] = '\0';
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

void dumpFrames(){
	tabla_de_frame_entry* frameEntry;
	int i;
	for(i=0; i< list_size(tablaDeFrames->entradas); i++){
		frameEntry = list_get(tablaDeFrames->entradas, i);
		printf("Frame= %d | Ocupado=%d | Direccion=%d\n",
				frameEntry->nroFrame, frameEntry->ocupado, frameEntry->direccion_real);
	}
	if(i==0){
		printf("No hay frames WTF\n");
	}
}

void dumpAllData(){
	int i;
	tabla_de_paginas* tablaActual;
	for(i = 0; i<list_size(tablasDePaginas); i++){
		tablaActual = list_get(tablasDePaginas, i);
		printData(tablaActual);
	}
	if(i==0){
		printf("No hay ninguna tabla de paginas\n");
	}
}

void dumpData(int pid){
	tabla_de_paginas* tabla = buscarPorPID(pid);
	if(tabla == NULL){
		printf("Fallo: tabla no encontrada");
	}
	printData(tabla);
}

void printfTLBEntry(tlb_entry* tlbEntry, int i){
	printf("Index %d | Pid %d | NroPagina %d | NroFrame %d | Ref %s \n", i,
			tlbEntry->pid, tlbEntry->nroPagina, tlbEntry->nroFrame, tlbEntry->last_use);
}

void dumpTlb(){
	if(!TLBEnable){
		printf("La TLB no esta habilitada\n");
		return;
	}

	printf("Mostrando TLB [Max size %d]\n", TLB->size);

	int i;
	tlb_entry* tlbEntry;
	for(i=0; i<list_size(TLB->entradas); i++){
		tlbEntry = list_get(TLB->entradas, i);
		printfTLBEntry(tlbEntry, i);
	}

	if(i==0){
		printf("La TLB no tiene entradas [Hey, por ahora...]\n");
	}
}

void tlbEntryDestroyer(void * entry){
	tlb_entry* tlbEntry = entry;
	free(tlbEntry->last_use);
	free(tlbEntry);
}

void flushTlb(){
	if(!TLBEnable){
		printf("La TLB no esta habilitada\n");
		return;
	}

	list_clean_and_destroy_elements(TLB->entradas, tlbEntryDestroyer);
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

