#include "tlb.h"
#include "commons/temporal.h"
#include <string.h>

tlb* TLB;
int TLBEnable;

pthread_mutex_t tlb_semaphore = PTHREAD_MUTEX_INITIALIZER;

void initTLB(t_config* config){
	int cantidad_entradas_tlb = config_get_int_value(config, "ENTRADAS_TLB");

	if(cantidad_entradas_tlb==0){
		log_info(logger, "TLB Deshabilitada");
		TLBEnable = 0;
		return;
	}

	TLBEnable = 1;

	TLB = malloc(sizeof(tlb));

	TLB->size = cantidad_entradas_tlb;
	TLB->entradas = list_create();

	log_info(logger, "[TLB] Creada TLB");
	log_info(logger, "[TLB] Cantidad de entradas maxima: %d", TLB->size);
	log_info(logger, "[TLB] Cantidad de entradas actual: %d", list_size(TLB->entradas));
}

int tlbTieneEntradasLibres(){
	return TLB->size >= TLB->entradas->elements_count;
}

//En este caso, la TLB tiene una o mas entradas libres
void llenarTLB(int nroPagina, int pid, int nroFrame){
	tlb_entry* entry = malloc(sizeof(tlb_entry));
	entry->pid = pid;
	entry->nroPagina = nroPagina;
	entry->nroFrame = nroFrame;
	entry->last_use = temporal_get_string_time();
	list_add(TLB->entradas, entry);

	log_trace(logger, "[TLB] Entrada agregada: pid=%d, page=%d, frame=%d, ref=%s", pid, nroPagina, nroFrame, entry->last_use);
}

//Retorna true si es verdad que la anterior es anterior a la posterior :P
int esFechaAnterior(char* anterior, char* posterior){
	return !strcmp(anterior, posterior);
}

void actualizarTLB(int nroPagina, int pid, int nroFrame){

	pthread_mutex_lock(&tlb_semaphore);
	if(tlbTieneEntradasLibres()){
		llenarTLB(nroPagina, pid, nroFrame);
		pthread_mutex_unlock(&tlb_semaphore);
		return;
	}
	pthread_mutex_unlock(&tlb_semaphore);


	//A partir de aca, es necesario hacer reemplazo de pagina
	int i = 0;
	tlb_entry* actual = list_get(TLB->entradas, i);
	tlb_entry* leastRecentlyUsed = actual; //victima
	for(i=1; i< TLB->size; i++){
		actual = list_get(TLB->entradas, i);
		if(esFechaAnterior(actual->last_use, leastRecentlyUsed->last_use)){
			leastRecentlyUsed = actual;
		}
	}

	log_warning(logger, "[TLB] Reemplazo: pid=%d, page=%d, frame=%d, ref=%s", leastRecentlyUsed->pid,
			leastRecentlyUsed->nroPagina, leastRecentlyUsed->nroFrame, leastRecentlyUsed->last_use);

	leastRecentlyUsed->nroFrame = nroFrame;
	leastRecentlyUsed->nroPagina = nroPagina;
	leastRecentlyUsed->pid = pid;
	leastRecentlyUsed->last_use = temporal_get_string_time();

	log_warning(logger, "[TLB] Se agrega: pid=%d, page=%d, frame=%d, ref=%s", leastRecentlyUsed->pid,
				leastRecentlyUsed->nroPagina, leastRecentlyUsed->nroFrame, leastRecentlyUsed->last_use);
}

int buscarEnTLB(int nroPagina, int pid){
	int i;
	tlb_entry* actual;
	for(i=0; i< TLB->entradas->elements_count; i++){
		actual = list_get(TLB->entradas, i);
		if(actual->pid == pid && actual->nroPagina == nroPagina){
			actual->last_use = temporal_get_string_time();
			log_trace(logger, "[TLB] Hit: pid=%d, page=%d, frame=%d, ref=%s", pid, nroPagina, actual->nroFrame, actual->last_use);
			return actual->nroFrame;
		}
	}

	log_trace(logger, "[TLB] Miss: pid=%d, page=%d", pid, nroPagina);
	return -1;
}

void destroyTlbEntry(void* entry){
	log_trace(logger, "Flush de TLB");

	tlb_entry* tlbEntry = (tlb_entry*) entry;
	free(tlbEntry->last_use);
	free(tlbEntry);
}

void flush(int pid){
	int i;
	tlb_entry* actual;
	for(i=0; i< TLB->entradas->elements_count; i++){
		actual = list_get(TLB->entradas, i);
		if(actual->pid == pid){
			list_remove_and_destroy_element(TLB->entradas, i, destroyTlbEntry);
		}
	}
}
