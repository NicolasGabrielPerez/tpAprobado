#ifndef PAGES_ADMIN_H_
#define PAGES_ADMIN_H_

#include "swap-structs.h"
#include <sockets/communication.h>

	extern int umc_socket;

	int initUmc(t_config* config);

	void recibirInitPrograma();

	void recibirPedidoPagina();

	void recibirEscrituraPagina();

	void recibirFinPrograma();

	void makeHandshake();

#endif /* PAGES_ADMIN_H_ */
