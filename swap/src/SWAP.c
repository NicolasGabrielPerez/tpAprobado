#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/txt.h>
#include <sockets/sockets.h>
#include "swap-library.h"

int umc_socket;
int page_size;

int32_t HEADER_SIZE = sizeof(int32_t);
int32_t RESPUESTA_OK = 10;
int32_t RESPUESTA_FAIL = -10;

int32_t HEADER_HANDSHAKE = 100;
int32_t HEADER_SOLICITAR_PAGINAS = 300;
int32_t HEADER_ALMACENAR_PAGINAS = 400;
int32_t HEADER_FIN_PROGRAMA = 600;

char* buscarPagina(int nroPagina, int pid){
	return 0;
}

char* escribirPagina(int nroPagina, int pid, char* buffer){
	return 0;
}

void recibirPedidoPagina(){
	int32_t nroPagina;
	int32_t pid;
	char* respuesta;

	if (recv(umc_socket, &nroPagina, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if (recv(umc_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	respuesta = buscarPagina(nroPagina, pid);

	if (send(umc_socket, respuesta, page_size, 0) == -1) {
			perror("send");
			exit(1);
	}
}

void recibirEscrituraPagina(){
	int32_t nroPagina;
	int32_t pid;
	char* buffer = malloc(page_size);
	char* respuesta;

	if (recv(umc_socket, &nroPagina, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if (recv(umc_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if (recv(umc_socket, buffer, page_size, 0) == -1) {
		perror("recv");
		exit(1);
	}

	respuesta = escribirPagina(nroPagina, pid, buffer);

	if (send(umc_socket, respuesta, page_size, 0) == -1) {
			perror("send");
			exit(1);
	}
}

void recibirFinPrograma(){

}

void makeHandshake(){

}

int esperarConexionUMC(int umc_listener){
	int umc_socket;
	int bytes_recibidos;
	char* header = malloc(HEADER_SIZE);
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;
	char remoteIP[INET6_ADDRSTRLEN];

	   // handle new connections
	   addrlen = sizeof remoteaddr;
	   umc_socket = accept(umc_listener,
		   (struct sockaddr *)&remoteaddr,
		   &addrlen);

	   puts("Conexion aceptada");
	   if (umc_socket == -1) {
		   perror("accept");
	   } else {

			printf("umc: new connection from %s on "
				"socket %d\n",
				inet_ntop(remoteaddr.ss_family,
					get_in_addr((struct sockaddr*)&remoteaddr),
					remoteIP, INET6_ADDRSTRLEN),
					umc_listener);

			printf("El fd es: %d", umc_socket);
			if ((bytes_recibidos = recv(umc_socket, header, HEADER_SIZE, 0)) == -1) {
			   perror("recv");
			   exit(1);
		   }

			makeHandshake();
	   }

	   free(header);
	   return umc_socket;
}

void initSwap(int swap_size){
	FILE* particion = txt_open_for_append("particion");
	int i;
	for(i=0;i<swap_size;i++){
		txt_write_in_file(particion, '\0');
	}
}

void operarSegunHeader(int32_t header){
	if(header == HEADER_SOLICITAR_PAGINAS){
		recibirPedidoPagina();
		return;
	}
	if(header == HEADER_ALMACENAR_PAGINAS){
		recibirEscrituraPagina();
		return;
	}
	if(header == HEADER_FIN_PROGRAMA){
		recibirFinPrograma();
		return;
	}
}

int main(void) {
	t_config* config = config_create("swap.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}
	char* puerto_umc = config_get_string_value(config, "PUERTO_UMC"); //puerto usado escuchar a la umc
	page_size =config_get_int_value(config, "TAMANIO_PAGINA");
	int cantidad_page =config_get_int_value(config, "CANTIDAD_PAGINAS");
//	int retardo_fragmentacion =config_get_int_value(config, "RETARDO_COMPACTACION");
	int swap_size = page_size * cantidad_page;


	printf("Config: PUERTO_UMC=%s\n", puerto_umc);
	printf("Config: SWAP_SIZE=%d\n", swap_size);

	initSwap(swap_size);

	int umc_listener = crear_puerto_escucha(puerto_umc); //socket usado escuchar a la umc
	umc_socket = esperarConexionUMC(umc_listener);

	int bytes_recibidos = 1;
	char* header = malloc(HEADER_SIZE);
	int32_t headerInt;
	while(bytes_recibidos){
		puts("Esperando conexiones...");

		bytes_recibidos = recv(umc_socket, header, HEADER_SIZE, 0);

		if(bytes_recibidos == -1) {
		   perror("recv");
		   exit(1);
		}

		 if (bytes_recibidos == 0) {
		   // connection closed
		   puts("umc hung up\n");
	   }

	   memcpy(&headerInt, header, sizeof(int32_t));
	   operarSegunHeader(headerInt);
	}

	close(umc_socket); // bye!

	puts("Terminé felizmente");
	return EXIT_SUCCESS;
}
