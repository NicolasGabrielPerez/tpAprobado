#ifndef CONSOLA_INTERFAZ_H_
#define CONSOLA_INTERFAZ_H_
#include "nucleo-structs.h"

	extern fd_set consola_sockets_set;
	extern int fd_consola_max;
	extern int consola_listener;

	void initConsolaListener(t_config* config);

	void manejarConexionesConsolas();

	void endOfProgram(int socket);

	void sendResults(int socket, char* result);

	void* header(int socket);
	// develve el header de fin de programa o el programa ansisop entrero
	// todo recivido de consola

#endif
