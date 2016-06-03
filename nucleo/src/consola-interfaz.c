#include "consola-interfaz.h"
#include <sockets/communication.h>

int consola_listener;
fd_set consola_sockets_set;
int fd_consola_max;

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
}

void actualizarFdMax(int socket){
	if(socket>fd_consola_max) fd_consola_max = socket;
}

void makeHandshake(int consola_socket){
	enviarOKConContenido(consola_socket,11,"Soy Nucleo");
}

void initPrograma(int consola_socket){

}

void handleConsolaRequest(int consola_socket){
	char* header = malloc(HEADER_SIZE);
	if (recv((int)socket, header, HEADER_SIZE, 0) == -1) {
		perror("recv");
		exit(1);
	}
	int32_t iHeader = convertToInt32(header);
	if(iHeader == HEADER_HANDSHAKE){
		makeHandshake(consola_socket);
		return;
	}
	if(iHeader == HEADER_INIT_PROGRAMA){
		initPrograma(consola_socket);
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
	   exit(4);
	}

	int i;
	for(i = 0; i <= fd_consola_max; i++) {
		manejarCambiosEnSocket(i, &read_fds);
	}
}
