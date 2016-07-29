#ifndef CONSOLA_INTERFAZ_H_
#define CONSOLA_INTERFAZ_H_

#include "nucleo-structs.h"
#include <sockets/communication.h>
#include <sockets/serialization.h>

	extern fd_set consola_sockets_set;
	extern int fd_consola_max;
	extern int consola_listener;

	void com_initConsolaListener(t_config* config);

	void console_endProgram(int socket);

	void console_sendResults(int socket, message* result);

	void handleConsoleRquests(int socket);

	char* nucleo_obtenerProgramaANSISOP(message* ANSISOP);

	void console_makeHandshake(int consola_socket);

	void initNewProgram(u_int32_t codeSize, char* programSourceCode, int consoleSocket);

	void* console_comunication_program();

#endif
