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

int main(void) {
	t_config* config = config_create("swap.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}
	char* puerto_umc = config_get_string_value(config, "PUERTO_UMC");

	printf("Config: PUERTO_UMC=%s\n", puerto_umc);

	int bytes_recibidos;
	char buf[50];

	int socket_umc = crear_socket_cliente("utnso40", puerto_umc); //socket usado para conectarse a la umc
	//Hago handskae con umc
	if(handshake(socket_umc, "PRUEBA") != 0){
		puts("Error en handshake con la umc");
	}

	printf("FD: %d", socket_umc);

	puts("Esperando conexiones...");
	//Quiero recibir de umc, lo que le pasó consola
	if ((bytes_recibidos = recv(socket_umc, buf, sizeof(buf), 0)) == -1) {
	   perror("recv");
	   exit(1);
	}

	printf("Recibidos %d bytes \n", bytes_recibidos);
	printf("Recibi lo siguiente de nucleo:\n%s\n", buf);

	puts("Terminé felizmente");
	return EXIT_SUCCESS;
}
