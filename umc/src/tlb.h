#ifndef TLB_H_
#define TLB_H_

#include "umc-structs.h"
#include <commons/collections/queue.h>

	extern int TLBEnable;

	typedef struct tlb_entry{
		int pid;
		int nroPagina;
		int frame;
	} tlb_entry;

	typedef struct tlb{
		tlb_entry* entradas;
		t_queue* usedPages; //cola de indices de las entradas
	} tlb;

	void initTLB(t_config* config);

	int actualizarTLB(int nroPagina, int pid);

	int buscarEnTLB(int nroPagina, int pid);

#endif /* TLB_H_ */
