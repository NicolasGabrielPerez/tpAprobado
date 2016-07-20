#ifndef PAGES_ADMIN_H_
#define PAGES_ADMIN_H_

#include "swap-structs.h"
#include <sockets/communication.h>

	extern int umc_socket;

	int initUmc(t_config* config);

	int existePid(int pid);

	void initPaginas(int pid, int cantPaginas, char* codFuente);

	response* getPagina(int nroPagina, int pid);

	response* escribirPagina(int nroPagina, int pid, char* buffer);

	int hayEspacioDisponible(int cantPaginas);

	response* finalizarPrograma(int pid);

	response* getPagina(int nroPagina, int pid);

	int cantFramesDisponibles();

	void initLogger();

#endif /* PAGES_ADMIN_H_ */
