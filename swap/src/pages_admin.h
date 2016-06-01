#ifndef PAGES_ADMIN_H_
#define PAGES_ADMIN_H_

#include "swap-structs.h"
#include <sockets/communication.h>

	extern int umc_socket;

	int initUmc(t_config* config);

	int existePid(int pid);

	void initPaginas(int pid, int cantPaginas, char* codFuente);

	char* getPagina(int nroPagina, int pid);

	int escribirPagina(int nroPagina, int pid, char* buffer);

	int hayEspacioDisponible(int cantPaginas);

	void recibirInitPrograma();

	void recibirPedidoPagina();

	void recibirEscrituraPagina();

	void recibirFinPrograma();

	void makeHandshake();

#endif /* PAGES_ADMIN_H_ */
