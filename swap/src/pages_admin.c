#include "pages_admin.h"

t_log* logger;

void initLogger(){
	logger = log_create("swap.log", "SWAP",true, LOG_LEVEL_TRACE);
	log_trace(logger, "---------------INIT LOG----------------");
}

char* leerDeParticion(int nroFrame, int size){
	char* buffer = malloc(size);
	fseek(swapAdmin->particion, nroFrame*paginaSize, SEEK_SET);
	log_trace(logger, "Frame a leer: %d. Posicion: %d", nroFrame, ftell(swapAdmin->particion));
	fread(buffer, sizeof(char), size, swapAdmin->particion);

	return buffer;
}

char* leerDeFrame(frame_entry* frameEntry){
	return leerDeParticion(frameEntry->nroFrame, paginaSize);
}

char* buscarPagina(int nroPagina, int pid){
	frame_entry* frameEntry = buscarFrameEntry(nroPagina, pid);
	return leerDeFrame(frameEntry);
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
	int i;
	frame_entry* entry;
	for(i = 0; i< cantPaginasSwap; i++){
		entry = getFrameEntryPorNroFrame(i);
		if(entry->pid == pid){
			return 1;
		}
	}
	return 0;
}

void setearFramesEntriesNuevoPid(int nroFrameNuevoPrograma, int pid, int cantPaginas){
	int espacioContiguoEnd = nroFrameNuevoPrograma + cantPaginas;
	int indiceFrame; //coincide con el nroFrame
	int indicePagina = 0;
	for(indiceFrame=nroFrameNuevoPrograma;
			indiceFrame<espacioContiguoEnd;
			indiceFrame++){
		frame_entry* frameEntry = getFrameEntryPorNroFrame(indiceFrame);
		frameEntry->nroPagina = indicePagina;
		indicePagina++;
		frameEntry->pid = pid;
	}
}

void escribirEnParticion(int nroFrame, char* buffer, int size){
	fseek(swapAdmin->particion, nroFrame*paginaSize, SEEK_SET);
	log_trace(logger, "Frame a escribir: %d. Posicion: %d", nroFrame, ftell(swapAdmin->particion));
	fwrite(buffer, sizeof(char), size, swapAdmin->particion);
	fflush(swapAdmin->particion);
}

response* escribirPagina(int nroPagina, int pid, char* buffer){
	//Buscar frame entry
	frame_entry* frameEntry = buscarFrameEntry(nroPagina, pid);
	if(frameEntry==NULL){
		return createFAILResponse(PID_NO_EXISTE);
	}

	//Escribir en particion
	escribirEnParticion(frameEntry->nroFrame, buffer, paginaSize);

	return createOKResponse();
}

void escribirNuevoPrograma(int pid, int cantPaginas, char* codFuente, int nroFrameNuevoPrograma){
	setearFramesEntriesNuevoPid(nroFrameNuevoPrograma, pid, cantPaginas);
	escribirEnParticion(nroFrameNuevoPrograma, codFuente, cantPaginas*paginaSize);
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

frame_entry* getNextUsed(int aPartirDe){
	int i;
	for(i=aPartirDe; i<cantPaginasSwap; i++){
		if(!frameDisponible(i)){
			return getFrameEntryPorNroFrame(i);
		}
	}
	return NULL;
}

void setearOcupado(int nroFrame){
	bitarray_set_bit(swapAdmin->bitMap, nroFrame);
}

void setearLibre(int nroFrame){
	bitarray_clean_bit(swapAdmin->bitMap, nroFrame);
}

int bajarNextUsed(int nroFrameDisponible){ //return -1 si no hay nada que bajar. Es decir, no hay ningun frame ocupado a partir de esa posicion
	frame_entry* nextUsed = getNextUsed(nroFrameDisponible);
	if(nextUsed == NULL) return -1;

	escribirEnParticion(nroFrameDisponible*paginaSize, leerDeFrame(nextUsed), paginaSize);
	setearOcupado(nroFrameDisponible);
	setearLibre(nextUsed->nroFrame);
	return 1;
}

void desfragmentarParticion(){
	int i;

	for(i = 0; i< cantPaginasSwap; i++){
		if(frameDisponible(i)){
			if(bajarNextUsed(i) == -1) return; //si no hay que seguir compactando => terminar desfragmentacion
		}
	}
}

void initPaginas(int pid, int cantPaginas, char* codFuente){
	int nroFrameNuevoPrograma = getEspacioContiguoStart(cantPaginas); //espacioContiguoStart
	if(nroFrameNuevoPrograma != -1){
		escribirNuevoPrograma(pid, cantPaginas, codFuente, nroFrameNuevoPrograma);
		return;
	}
	//Ya viene verificado que existe espacio disponible para esa cantidad de paginas
	//Pero en este caso habria que desfragmentar para poder reservar espacio contiguo
	desfragmentarParticion();
	nroFrameNuevoPrograma = getEspacioContiguoStart(cantPaginas);
	escribirNuevoPrograma(pid, cantPaginas, codFuente, nroFrameNuevoPrograma);
}

response* getPagina(int nroPagina, int pid){
	int i;

	if(!existePid(pid)){
		return createFAILResponse(PID_NO_EXISTE);
	}

	frame_entry* entry;
	for(i = 0; i< cantPaginasSwap; i++){
		entry = getFrameEntryPorNroFrame(i);
		if(entry->pid == pid && entry->nroPagina == nroPagina){
			return createResponse(1, 0, paginaSize, leerDeFrame(entry));
		}
	}

	return createFAILResponse(PAGINA_NO_EXISTE);
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
