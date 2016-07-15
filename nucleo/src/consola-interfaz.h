#ifndef CONSOLA_INTERFAZ_H_
#define CONSOLA_INTERFAZ_H_

#include "nucleo-structs.h"
#include <sockets/communication.h>
#include <sockets/serialization.h>

	extern fd_set consola_sockets_set;
	extern int fd_consola_max;
	extern int consola_listener;

	void initConsolaListener(t_config* config);

	void manejarConexionesConsolas();

	void console_endProgram(int socket);

	void sendResults(int socket, char* result);

	void handleConsoleRquests(int socket);

	char* recibirProgramaANSISOP(message* ANSISOP);

	void makeHandshake(int consola_socket);

	void finalizarFelizmenteTodo(int processID);

	void initNewProgram(u_int32_t codeSize, char* programSourceCode, int consoleSocket);

#endif
