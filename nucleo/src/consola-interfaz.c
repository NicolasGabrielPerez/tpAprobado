#include "consola-interfaz.h"
#include <sockets/sockets.h>
#include "umc-interfaz.h"


int consola_listener;
fd_set consola_sockets_set;
int fd_consola_max;

void header(int socket){
	message* program;
	program = receiveMessage(socket);

	//Saqué el switch case porque googleé que no funciona con constantes definidas como las usamos nosotros. La solución, el if...

	if(program->header == HEADER_HANDSHAKE){
		makeHandshake(socket);
	}

	if(program->header == HEADER_FIN_PROGRAMA){
		finalizarFelizmenteTodo(socket);
	}
	else{
		if(program->header == HEADER_INIT_PROGRAMA){
			char* programaANSISOP = recibirProgramaANSISOP(program);
			initNewProgram(programaANSISOP, socket);
			free(programaANSISOP);
			free(program);
		}
		else{
			perror("header invalido");
			enviarFAIL(socket, HEADER_INVALIDO);
		}
	}
}

void finalizarFelizmenteTodo(int socket){
	notificarFinDePrograma(socket);
	//TODO sacar de cpu en ejecucion
}

// falta una parte
void initNewProgram(char* ANSiSop){
	PCB* nuevoPCB;
	nuevoPCB = new_pcb();
	int cantPage;
	cantPage = getProgramPagesCount(ANSiSop);
	create_program_PCB(nuevoPCB, ANSiSop,cantPage);

	almacenamientoPosible(cantPage,nuevoPCB,ANSiSop);

}

void endOfProgram(int socket){
	sendMessage(socket, HEADER_FIN_PROGRAMA, 0, "");
}
// SEND

int reciveEndOfProgram(message end){
	return HEADER_FIN_PROGRAMA;
}

void sendResults(int socket, char* result){
	int	contenidoSize = sizeof(result);
	int header = HEADER_RESULTADOS;

	sendMessage( socket, header, contenidoSize,  result);
}
// SEND

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

//hecho arriba mas completo
void initPrograma(char* program){}
//hecho arriba mas completo
void handleConsolaRequest(int consola_socket){
	message* message = receiveMessage(consola_socket);

	if(message->header == HEADER_HANDSHAKE){
		makeHandshake(consola_socket);

		return;
	}

	if(message->header == HEADER_INIT_PROGRAMA){
		//TODO:testear cómo viene el programa desde la consola.
		char* program = malloc(message->contenidoSize);
		memcpy(program, message->contenido, message->contenidoSize);
		initPrograma(program);

		free(program);
		return;
	}
	enviarFAIL(consola_socket, HEADER_INVALIDO);
}

void manejarCambiosEnSocket(int socket, fd_set* read_set){
	if (FD_ISSET(socket, read_set)) {
	   if (socket == consola_listener) { //nuevo cpu
			int new_fd = aceptarNuevaConexion(consola_listener);
			actualizarFdMax(new_fd);
	   } else{
		   handleConsolaRequest(socket);
	   }
   };
}

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
