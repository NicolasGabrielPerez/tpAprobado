#include "comandos.h"

void dumpFrame(int nroFrame){
	frame_entry* frameEntry = list_get(swapAdmin->framesEntries, nroFrame);
	int ocupado = bitarray_test_bit(swapAdmin->bitMap, nroFrame);
	printf("Nro frame: %d | Ocupado: %d | Pid: %d | Pag: %d \n", nroFrame, ocupado, frameEntry->pid, frameEntry->nroPagina);
}

void dumpTable(){
	int i;
	printf("--------------------- Dump de frames ---------------------\n");
	for(i=0; i< cantPaginasSwap; i++){
		dumpFrame(i);
	}
	printf("------------------- Fin Dump de frames -------------------\n");
}

