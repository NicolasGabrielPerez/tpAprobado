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
#include <comunicacion/sockets.h>

#define MAXDATASIZE 100 // max number of bytes we can get at once

int main(void) {
	t_config* config = config_create("consola.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}
	char* puerto_nucleo = config_get_string_value(config, "PUERTO_NUCLEO");

	printf("Config: PUERTO_NUCLEO=%s\n", puerto_nucleo);

	int received_bytes;
	char buf[50];
	puts("Ingrese comando\n");
	scanf("%s", buf);

	printf("Ejecutando...\n", buf);

	int socket_nucleo = crear_socket_cliente("utnso40", puerto_nucleo); //socket usado para conectarse a la umc

	//Hago handskae con umc
	if(handshake(socket_nucleo, "PRUEBA") != 0){
		puts("Error en handshake con el Núcleo");
	}

	//Le envio a nucleo el texto ingresado por consola
	if (send(socket_nucleo, buf, sizeof(buf), 0) == -1) {
		 perror("send");
	}

	puts("Se envió comando al Núcleo\n");

//	while(1){
//		puts("Esperando conexiones...");
//		if ((received_bytes = recv(socket_nucleo, buf, 50, 0)) == -1) {
//			perror("recv");
//			exit(1);
//		}
//	}

	puts("Terminé felizmente");

	return EXIT_SUCCESS;
}
