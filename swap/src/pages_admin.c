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

char* escribirPagina(int nroPagina, int pid, char* buffer){
	return 0;
}

int hayEspacioDisponible(int cantPaginas){
	return 0;
}

void escribirPaginas(int pid, int cantPaginas, char* codFuente){
	return;
}
