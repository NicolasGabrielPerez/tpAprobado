#ifndef SWAP_INTERFAZ_H_
#define SWAP_INTERFAZ_H_

#include "umc-structs.h"
#include <sockets/communication.h>

	extern int swap_socket;
	extern pthread_mutex_t swap_semaphore;

	//Hace handshake con swap
	void initSwap(t_config* config);

	response* initProgramaSwap(int* pid, int* cantPaginas, char* codFuente);

	response* finalizarProgramaSwap(int* pid);

	response* pedirPaginaASwap(int nroPagina, int pid);

	response* escribirPaginaEnSwap(int nroPagina, int pid, char* contenido);

#endif /* SWAP_INTERFAZ_H_ */
