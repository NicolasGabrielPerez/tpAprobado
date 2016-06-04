#include "cpu-interfaz.h"
#include <sockets/communication.h>
#include <sockets/serialization.h>

int cpu_listener;
fd_set cpu_sockets_set;
int fd_cpu_max;

void initCPUListener(t_config* config){
	char* puerto_cpu = config_get_string_value(config, "PUERTO_CPU");
    cpu_listener = crear_puerto_escucha(puerto_cpu);
    FD_ZERO(&cpu_sockets_set);    // clear the master and temp sets
	FD_SET(cpu_listener, &cpu_sockets_set);
	fd_cpu_max = cpu_listener;
}

void actualizarFdCPUMax(int socket){
	if(socket>fd_cpu_max) fd_cpu_max = socket;
}

void makeHandshakeWithCPU(int cpu_socket){
	enviarOKConContenido(cpu_socket, 11, "Soy Nucleo");
}

void finPrograma(int cpu_socket){

}

void handleCPURequest(int cpu_socket){
	char* header = malloc(HEADER_SIZE);
	if (recv((int)socket, header, HEADER_SIZE, 0) == -1) {
		perror("recv");
		exit(1);
	}
	int32_t iHeader = convertToInt32(header);
	if(iHeader == HEADER_HANDSHAKE){
		makeHandshakeWithCPU(cpu_socket);
		return;
	}
	if(iHeader == FIN_PROGRAMA){
		finPrograma(cpu_socket);
		return;
	}

	enviarFAIL(cpu_socket, HEADER_INVALIDO);
}

void manejarSocketChanges(int socket, fd_set* read_set){
	if (FD_ISSET(socket, read_set)) {
	   if (socket == cpu_listener) { //nuevo cpu
			int new_fd = aceptarNuevaConexion(cpu_listener);
			actualizarFdCPUMax(new_fd);
	   } else{
		   handleCPURequest(socket);
	   }
   };
}

void manejarConexionesCPUs(){
	fd_set read_fds; //set auxiliar
	read_fds = cpu_sockets_set;
	if (select(fd_cpu_max+1, &read_fds, NULL, NULL, NULL) == -1) {
	   perror("select");
	   exit(4);
	}
	int i;
	for(i = 0; i <= fd_cpu_max; i++) {
		manejarSocketChanges(i, &read_fds);
	};
}