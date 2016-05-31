#include "tlb.h"

int cantidad_entradas_tlb;
tlb* TLB;
int TLBEnable;

void initTLB(t_config* config){
	cantidad_entradas_tlb = config_get_int_value(config, "ENTRADAS_TLB");

	if(cantidad_entradas_tlb==0) return;

	TLB = malloc(sizeof(tlb));
	TLB->entradas = malloc(sizeof(tlb_entry)*cantidad_entradas_tlb);
	int i;
	for(i=0;i<cantidad_entradas_tlb;i++){
		tlb_entry* entrada = malloc(sizeof(tlb_entry));
		entrada->frame = 0;
		entrada->nroPagina = 0;
		entrada->pid = 0;
		TLB->entradas[i] = *entrada;
		free(entrada);
	}
	TLB-> usedPages = queue_create();
	TLBEnable = 1;
}

int actualizarTLB(int nroPagina, int pid){
	return 0;
}

int buscarEnTLB(int nroPagina, int pid){
	return 0;
}
