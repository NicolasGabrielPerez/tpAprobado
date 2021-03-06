#include "consola-interfaz.h"
#include <sockets/sockets.h>
#include "umc-interfaz.h"


int consola_listener;
fd_set consola_sockets_set;
int fd_consola_max;

fd_set read_fds; //set auxiliar

//Recibe mensajes y llama a las funciones correspondientes según el HEADER
void handleConsoleRquests(int consoleSocket){
	message* consoleMessage = receiveMessage(consoleSocket);

	if(consoleMessage->codError == SOCKET_DESCONECTADO){
		//log_warning(nucleo_logger, "Socket de CONSOLA %d desconectado\n", consoleSocket);
		disconnect_console(consoleSocket);
		return;
	}

	if(consoleMessage->header == HEADER_HANDSHAKE){
		console_makeHandshake(consoleSocket);
		log_trace(nucleo_logger, "COMUNICACIÓN: Nueva consola conectada en %d", consoleSocket);
	}

	if(consoleMessage->header == HEADER_FIN_PROGRAMA){
		log_trace(nucleo_logger, "COMUNICACIÓN: Fin de programa recibido desde consola %d", consoleSocket);
		disconnect_console(consoleSocket);
	}
	if(consoleMessage->header == HEADER_INIT_PROGRAMA){
		int imp = consoleSocket;
		log_trace(nucleo_logger, "COMUNICACIÓN: Nuevo programa recibido de consola %d", imp);
		char* programaANSISOP = consoleMessage->contenido;
		initNewProgram(consoleMessage->contenidoSize, programaANSISOP, consoleSocket);
		free(programaANSISOP);
		free(consoleMessage);
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
	if(is_program_alive(socket)){
		sendMessage(socket, HEADER_RESULTADOS, result->contenidoSize, result->contenido);
	}
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

			FD_SET(new_fd, &consola_sockets_set);
		} else{
			//Cambio en socket consola => interpretar los mensajes de forma apropiada
			handleConsoleRquests(socket);
		}
	};
}

void* console_comunication_program(){

	while(1){
		read_fds = consola_sockets_set;

		if (select(fd_consola_max+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}
		int i;
		for(i = 0; i <= fd_consola_max; i++) {
			com_consoleManejarCambiosEnSocket(i, &read_fds);
		}
	}
}
//---------------------------------------------- </COMMUNICATION>

void disconnect_console(int consoleSocket){
	//Obtener PCB asociado a consola
	PCB* pcbToKill = remove_pcb_by_ID(General_Process_List, consoleSocket);
	//Eliminar pcb de todas las estructuras
	remove_pcb_by_ID(RUNNING_Process_List, consoleSocket);

	end_process(pcbToKill->processId);						//Destruye las estructuras del proceso dentro del núcleo
	FD_CLR(consoleSocket, &consola_sockets_set);
	//close(consoleSocket);
	//Que pasa si hago free de un pcb que está en una cola o lista???
	//Si está ejecutando, que pasa si quiero actualizar al volver el pcb desde CPU???
}


