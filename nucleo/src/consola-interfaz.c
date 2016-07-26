#include "consola-interfaz.h"
#include <sockets/sockets.h>
#include "umc-interfaz.h"


int consola_listener;
fd_set consola_sockets_set;
int fd_consola_max;

fd_set read_fds; //set auxiliar

//Recibe mensajes y llama a las funciones correspondientes según el HEADER
void handleConsoleRquests(int consoleSocket){
	message* message = receiveMessage(consoleSocket);

	if(message->header == HEADER_HANDSHAKE){
		console_makeHandshake(consoleSocket);
		log_trace(nucleo_logger, "COMUNICACIÓN: Nueva consola conectada en %d", consoleSocket);
	}

	if(message->header == HEADER_FIN_PROGRAMA){
		log_trace(nucleo_logger, "COMUNICACIÓN: Fin de programa recibido desde consola %d", consoleSocket);
		FD_CLR(consoleSocket, &read_fds);
		close(consoleSocket);
		set_pcb_EXIT(consoleSocket);		//consoleSocket == PID
	}
	if(message->header == HEADER_INIT_PROGRAMA){
		log_trace(nucleo_logger, "COMUNICACIÓN: Nuevo programa recibido de consola %d", consoleSocket);
		char* programaANSISOP = message->contenido;
		initNewProgram(message->contenidoSize, programaANSISOP, consoleSocket);
		free(programaANSISOP);
		free(message);
	}
	/*
	else{
		perror("header invalido");
		enviarFAIL(consoleSocket, HEADER_INVALIDO);
	}*/

	//if(message->contenidoSize > 0) free(message->contenido);
	//free(message);
}

//---------------------------------------------- <SEND>
//Envía mensaje de finalización de programa
void console_endProgram(int socket){
	sendMessage(socket, HEADER_FIN_PROGRAMA, 0, "");
	FD_CLR(socket, &read_fds);
	close(socket);
}

//Envía mensajes a la consola con lo que debe mostrar en pantalla
void console_sendResults(int socket, message* result){
	sendMessage(socket, HEADER_RESULTADOS, result->contenidoSize, result->contenido);
}
//---------------------------------------------- </SEND>

//---------------------------------------------- <RECEIVE>
//Devuelve el programa serializado recibido en un mensaje
char* nucleo_obtenerProgramaANSISOP(message* ANSISOP){
	char* program = malloc(ANSISOP->contenidoSize);
	memcpy(program, ANSISOP->contenido, ANSISOP->contenidoSize);

	return program;
}
//---------------------------------------------- </RECEIVE>

//---------------------------------------------- <COMMUNICATION>
void com_initConsolaListener(t_config* config){
	char* puerto_prog = config_get_string_value(config, "PUERTO_PROG");
	consola_listener = crear_puerto_escucha(puerto_prog);
	FD_ZERO(&consola_sockets_set);    // clear the master and temp sets
	FD_SET(consola_listener, &consola_sockets_set);
	fd_consola_max = consola_listener;
}

void actualizarFdMax(int socket){
	if(socket>fd_consola_max) fd_consola_max = socket;
}

void console_makeHandshake(int consola_socket){
	sendMessage(consola_socket, HEADER_HANDSHAKE, 0, NULL);
}

void com_consoleManejarCambiosEnSocket(int socket, fd_set* read_set){
	if (FD_ISSET(socket, read_set)) {
		if (socket == consola_listener) {
			int new_fd = aceptarNuevaConexion(consola_listener);
			actualizarFdMax(new_fd);
			handleConsoleRquests(new_fd);

			FD_SET(new_fd, read_set);
		} else{
			//Cambio en socket consola => interpretar los mensajes de forma apropiada
			handleConsoleRquests(socket);
		}
	};
}

//Select principal de consolas
void com_manejarConexionesConsolas(){
	fd_set read_fds; //set auxiliar
	read_fds = consola_sockets_set;
	if (select(fd_consola_max+1, &read_fds, NULL, NULL, NULL) == -1) {
		perror("select");
		//exit(4);
	}

	int i;
	for(i = 0; i <= fd_consola_max; i++) {
		com_consoleManejarCambiosEnSocket(i, &read_fds);
	}
}

void* console_comunication_program(){
	read_fds = consola_sockets_set;

	while(1){

		if (select(fd_consola_max+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}
		int i;
		for(i = 0; i <= fd_consola_max; i++) {
			com_consoleManejarCambiosEnSocket(i, &read_fds);
		};
	}

	/*while(1){
		manejarConexionesConsolas();
	}*/
}
//---------------------------------------------- </COMMUNICATION>

int convertToInt32(char* buffer){
	int32_t* number = malloc(sizeof(int32_t));
	memcpy(number, buffer, sizeof(int32_t));

	int32_t numberToReturn = *number;
	return numberToReturn;
}
