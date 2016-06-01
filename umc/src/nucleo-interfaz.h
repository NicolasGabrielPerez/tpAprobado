#ifndef NUCLEO_INTERFAZ_H_
#define NUCLEO_INTERFAZ_H_
#include "umc-structs.h"
#include <sockets/communication.h>

	void recbirInitPrograma(int nucleo_socket);

	void recibirFinalizarPrograma(int nucleo_socket);

#endif /* NUCLEO_INTERFAZ_H_ */
