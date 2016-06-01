#ifndef SWAP_INTERFAZ_H_
#define SWAP_INTERFAZ_H_

#include "umc-structs.h"

	extern int swap_socket;
	extern pthread_mutex_t swap_semaphore;

	//Hace handshake con swap
	void initSwap(t_config* config);

	char* initProgramaSwap(int* pid, int* cantPaginas, char* codFuente);

	char* finalizarProgramaSwap(int* pid);

	char* pedirPaginaASwap(int nroPagina, int pid);

#endif /* SWAP_INTERFAZ_H_ */
