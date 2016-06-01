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

response* escribirPagina(int nroPagina, int pid, char* buffer){
	frame_entry* frameEntry = buscarFrameEntry(nroPagina, pid);
	if(frameEntry==NULL){
		return createFAILResponse(PID_NO_EXISTE);
	}
	int offset = frameEntry->nroFrame * paginaSize;
	fseek(swapAdmin->particion, SEEK_SET + offset, 0);
	fwrite(buffer, paginaSize, 1, swapAdmin->particion);
	return createOKResponse();
}

int frameDisponible(int nroFrame){
	return !bitarray_test_bit(swapAdmin->bitMap, nroFrame);
}

int cantFramesDisponibles(){
	int i;
	int contador = 0;
	for(i=0;i<cantPaginasSwap;i++){
		if(frameDisponible(i)) contador++;
	}
	return contador;
}

int hayEspacioDisponible(int cantPaginas){
	return cantFramesDisponibles() >= cantPaginas;
}

int existePid(int pid){
	return 0;
}

void setearFramesEntriesNuevoPid(int espacioContiguoStart, int pid){
	int espacioContiguoEnd = espacioContiguoStart + cantPaginasSwap;
	int indiceFrame; //coincide con el nroFrame
	int indicePagina = 0;
	for(indiceFrame=espacioContiguoStart;
			indiceFrame<espacioContiguoEnd;
			indiceFrame++){
		frame_entry* frameEntry = getFrameEntryPorNroFrame(indiceFrame);
		frameEntry->nroPagina = indicePagina;
		indicePagina++;
		frameEntry->pid = pid;
	}
}

void escribirEnParticion(int offset, char* buffer, int size){
	fwrite(buffer, size, 1, swapAdmin->particion);
}

void escribirPaginas(int pid, int cantPaginas, char* codFuente, int espacioContiguoStart){
	setearFramesEntriesNuevoPid(espacioContiguoStart, pid);
	escribirEnParticion(espacioContiguoStart, codFuente, cantPaginas*paginaSize);
}

int hayEspacioContiguo(int indice, int cantPaginas){
	int i;
	int end = indice+cantPaginas;
	int contadorEspacioContiguo;
	for(i=indice; i<end; i++){
		if(frameDisponible(i)){
			contadorEspacioContiguo++;
		} else{
			return false;
		}
	}
	return true;
}

int getEspacioContiguoStart(int cantPaginas){
	int i;
	for(i=0;i<cantPaginasSwap;i++){
		if(hayEspacioContiguo(i, cantPaginas)) return i;
	}
	return -1;
}

void desfragmentarParticion(){
	return;
}

void initPaginas(int pid, int cantPaginas, char* codFuente){
	int espacioContiguoStart = getEspacioContiguoStart(cantPaginas);
	if(espacioContiguoStart != -1){
		escribirPaginas(pid, cantPaginas, codFuente, espacioContiguoStart);
		return;
	}
	//Ya viene verificado que existe espacio disponible para esa cantidad de paginas
	//Pero en este caso habria que desfragmentar para poder reservar espacio contiguo
	desfragmentarParticion();
	espacioContiguoStart = getEspacioContiguoStart(cantPaginas);
	escribirPaginas(pid, cantPaginas, codFuente, espacioContiguoStart);
}

response* getPagina(int nroPagina, int pid){
	return NULL;
}

void liberarFrame(int nroFrame){
	bitarray_clean_bit(swapAdmin->bitMap, nroFrame);
}

response* finalizarPrograma(int pid){
	t_list* entries = buscarEntries(pid);
	if(entries==NULL){
		return createFAILResponse(PID_NO_EXISTE);
	}
	int i;
	for(i=0;i<list_size(entries);i++){
		frame_entry* frameEntry = list_get(entries, i);
		frameEntry->pid = 0;
		liberarFrame(frameEntry->nroFrame);
	}
	return createOKResponse();
}
