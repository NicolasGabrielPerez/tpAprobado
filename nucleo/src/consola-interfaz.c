#include "consola-interfaz.h"
#include <sockets/sockets.h>
#include "umc-interfaz.h"


int consola_listener;
fd_set consola_sockets_set;
int fd_consola_max;

//Ex header()
//Recibe mensajes y llama a las funciones correspondientes según el HEADER
void handleConsoleRquests(int consoleSocket){
	message* message;
	message = receiveMessage(consoleSocket);

	if(message->header == HEADER_HANDSHAKE){
		makeHandshake(consoleSocket);
	}

	if(message->header == HEADER_FIN_PROGRAMA){
		finalizarFelizmenteTodo(consoleSocket);		//consoleSocket == PID
	}
	if(message->header == HEADER_INIT_PROGRAMA){
		char* programaANSISOP = recibirProgramaANSISOP(message);
		initNewProgram(message->contenidoSize, programaANSISOP, consoleSocket);
		free(programaANSISOP);
		free(message);
	}
	/*
	else{
		perror("header invalido");
		enviarFAIL(consoleSocket, HEADER_INVALIDO);
	}*/

}

void finalizarFelizmenteTodo(int processID){
	umc_notificarFinDePrograma(processID);		//Notifica fin de programa a UMC
	end_process(processID);						//Destruye las estructuras del proceso dentro del núcleo
	console_endProgram(processID);			//Notifica fin de programa a Consola
}

//Envía mensaje de finalización de programa
void console_endProgram(int socket){
	sendMessage(socket, HEADER_FIN_PROGRAMA, 0, "");
}
// SEND

int reciveEndOfProgram(message end){
	return HEADER_FIN_PROGRAMA;
}

//Envía mensajes a la consola con lo que debe mostrar en pantalla
void sendResults(int socket, char* result){
	int	contenidoSize = sizeof(result);
	int header = HEADER_RESULTADOS;

	sendMessage( socket, header, contenidoSize,  result);
}
// SEND

//Devuelve el programa serializado recibido en un mensaje
char* recibirProgramaANSISOP(message* ANSISOP){
	char* program = malloc(ANSISOP->contenidoSize);
	memcpy(program, ANSISOP->contenido, ANSISOP->contenidoSize);

	return program;
}

int convertToInt32(char* buffer){
	int32_t* number = malloc(sizeof(int32_t));
	memcpy(number, buffer, sizeof(int32_t));
	return *number;
}

void initConsolaListener(t_config* config){
	char* puerto_prog = config_get_string_value(config, "PUERTO_PROG");
	consola_listener = crear_puerto_escucha(puerto_prog);
	FD_ZERO(&consola_sockets_set);    // clear the master and temp sets
	FD_SET(consola_listener, &consola_sockets_set);
	fd_consola_max = consola_listener;
}

void actualizarFdMax(int socket){
	if(socket>fd_consola_max) fd_consola_max = socket;
}

void makeHandshake(int consola_socket){
	sendMessage(consola_socket, HEADER_HANDSHAKE, 0, NULL);
}

void manejarCambiosEnSocket(int socket, fd_set* read_set){
	if (FD_ISSET(socket, read_set)) {
		if (socket == consola_listener) { //nuevo cpu
			int new_fd = aceptarNuevaConexion(consola_listener);
			actualizarFdMax(new_fd);
		} else{
			//Cambio en socket consola => interpretar los mensajes de forma apropiada
			handleConsoleRquests(socket);
		}
	};
}

//Select principal de consolas
void manejarConexionesConsolas(){
	fd_set read_fds; //set auxiliar
	read_fds = consola_sockets_set;
	if (select(fd_consola_max+1, &read_fds, NULL, NULL, NULL) == -1) {
		perror("select");
		//exit(4);
	}

	int i;
	for(i = 0; i <= fd_consola_max; i++) {
		manejarCambiosEnSocket(i, &read_fds);
	}
}

void* console_comunication_program(){
	fd_set read_fds; //set auxiliar
		read_fds = consola_sockets_set;

		while(1){

			if (select(fd_cpu_max+1, &read_fds, NULL, NULL, NULL) == -1) {
				perror("select");
				exit(4);
			}
			int i;
			for(i = 0; i <= fd_cpu_max; i++) {
				manejarSocketChanges(i, &read_fds);
			};
		}

	/*while(1){
		manejarConexionesConsolas();
	}*/
}
