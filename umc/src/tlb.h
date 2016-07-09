#ifndef TLB_H_
#define TLB_H_

#include "umc-structs.h"
#include <commons/collections/queue.h>

	extern int TLBEnable;

	typedef struct tlb_entry{
		int pid;
		int nroPagina;
		int nroFrame;
		char* last_use;
	} tlb_entry;

	typedef struct tlb{
		t_list* entradas;
		int size; //cantidad de entradas
	} tlb;

	void initTLB(t_config* config);

	void actualizarTLB(int nroPagina, int pid, int nroFrame);

	int buscarEnTLB(int nroPagina, int pid);

	void flush(int pid);

#endif /* TLB_H_ */
