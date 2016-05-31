#include "pages_admin.h"

char* leerDeFrame(frame_entry* frameEntry){
	char* buffer = malloc(paginaSize);
	int offset = frameEntry->nroFrame * paginaSize;
	fseek(swapAdmin->particion, SEEK_SET + offset, 0);
	fread(buffer, paginaSize, 1, swapAdmin->particion);
	return buffer;
}

char* buscarPagina(int nroPagina, int pid){
	frame_entry* frameEntry = buscarFrameEntry(nroPagina, pid);
	return leerDeFrame(frameEntry);
}

int escribirPagina(int nroPagina, int pid, char* buffer){
	frame_entry* frameEntry = buscarFrameEntry(nroPagina, pid);
	int offset = frameEntry->nroFrame * paginaSize;
	fseek(swapAdmin->particion, SEEK_SET + offset, 0);
	fwrite(buffer, paginaSize, 1, swapAdmin->particion);
	return EXIT_SUCCESS;;
}

int cantidadDisponibles(){
	int i;
	int contador = 0;
	int bitArraySize = bitarray_get_max_bit(swapAdmin->bitMap);
	for(i=0;i<bitArraySize;i++){
		if(!bitarray_test_bit(swapAdmin->bitMap, i)) contador++;
	}
	return contador;
}

int hayEspacioDisponible(int cantPaginas){
	return cantidadDisponibles() >= cantPaginas;
}

void escribirPaginas(int pid, int cantPaginas, char* codFuente){
	return;
}
