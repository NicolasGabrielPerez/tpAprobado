#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <commons/config.h>
#include <sockets/sockets.h>
#include "swap-library.h"

int esperarConexionUMC(int umc_listener){
	int umc_socket;
	int bytes_recibidos;
	char buf[50];
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
			if ((bytes_recibidos = recv(umc_socket, buf, 50, 0)) == -1) {
			   perror("recv");
			   exit(1);
		   }

			printf("Se recibio: %s\nbytes_recibidos: %d.\n", buf, bytes_recibidos);

			if (send(umc_socket, "Soy SWAP", 50, 0) == -1) {
				 perror("send");
			 }

			puts("Termino el handshake\n");
	   }

	   return umc_socket;
}

int main(void) {
	t_config* config = config_create("swap.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}
	char* puerto_umc = config_get_string_value(config, "PUERTO_UMC"); //puerto usado escuchar a la umc
	int swap_size = config_get_int_value(config, "SWAP_SIZE");

	printf("Config: PUERTO_UMC=%s\n", puerto_umc);
	printf("Config: SWAP_SIZE=%d\n", swap_size);

	t_swap_block* swap = init_swap(swap_size);

	if(swap == NULL){
		puts("No se pudo inicializar SWAP");
	} else{
		printf("Inicializacion exitosa!!\n");
		printf("Size: %d\n", swap->size);
		printf("Primer char: %d\n", swap->memory_block[0]);
		printf("Disponible: %d\n", swap->disponible);
	}

	int umc_listener = crear_puerto_escucha(puerto_umc); //socket usado escuchar a la umc
	int umc_socket = esperarConexionUMC(umc_listener);

	int bytes_recibidos = 1;
	char buf[50];
	while(bytes_recibidos){
		puts("Esperando conexiones...");
		//Quiero recibir de umc, lo que le pasó consola
		bytes_recibidos = recv(umc_socket, buf, sizeof(buf), 0);

		if(bytes_recibidos == -1) {
		   perror("recv");
		   exit(1);
		}

		 if (bytes_recibidos == 0) {
		   // connection closed
		   puts("umc hung up\n");
	   }

		printf("Recibidos %d bytes \n", bytes_recibidos);
		printf("Recibi lo siguiente de nucleo:\n%s\n", buf);
	}

	close(umc_socket); // bye!

	puts("Terminé felizmente");
	return EXIT_SUCCESS;
}
