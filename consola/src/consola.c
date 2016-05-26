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
#include <commons/string.h>
#include <sockets/sockets.h>

#define MAXDATASIZE 100 // max number of bytes we can get at once

int main(int argc, char **argv) {
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


	//inicio interprete
	char* file;

	FILE *fp;



int  length =string_length(argv[1]);
      file = string_substring (argv[1], 2 , length);


	fp = fopen ( file , "r" );
	if (fp==NULL) {fputs ("File error",stderr); exit (1);}

	fseek(fp, 0, SEEK_END);
	double len= ftell(fp);
	fseek(fp, 0L, SEEK_SET);

char *paquete = malloc(len);

fscanf(fp, "%s" , paquete);

send_dinamic( socket_nucleo, paquete, len);



	puts("Terminé felizmente");

	return EXIT_SUCCESS;
}
