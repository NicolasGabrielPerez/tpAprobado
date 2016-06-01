#ifndef INTERFAZ_UMC_H_
#define INTERFAZ_UMC_H_

#include "pages_admin.h"
#include <sockets/communication.h>

	extern int umc_socket;

	int initUmc(t_config* config);

	void recibirInitPrograma();

	void recibirPedidoPagina();

	void recibirEscrituraPagina();

	void recibirFinPrograma();

	void makeHandshake();

#endif /* INTERFAZ_UMC_H_ */
