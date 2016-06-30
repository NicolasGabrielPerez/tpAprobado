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
#include <sockets/communication.h>



#define MAXDATASIZE 100 // max number of bytes we can get at once

void espera_resultados(int socketCliente){

	response* nucleoResponse = recibirResponse(socketCliente);
		// deserialize response


	if (nucleoResponse->ok > 0 ){

		char* mensaje = malloc(nucleoResponse->contenidoSize);
		mensaje = (nucleoResponse->contenido);
		printf("%s", mensaje);
		}

	else{
			perror("faill respounse");
		}

	}




int main(int argc, char **argv) {
	t_config* config = config_create("consola.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}
	char* puerto_nucleo = config_get_string_value(config, "PUERTO_NUCLEO");

	printf("Config: PUERTO_NUCLEO=%s\n", puerto_nucleo);


	int socket_nucleo = crear_socket_cliente("utnso40", puerto_nucleo);

	//Hago handskae con umc
	if(handshake(socket_nucleo, "PRUEBA") != 0){
		puts("Error en handshake con el Núcleo");
	}

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
free(paquete);


espera_resultados(socket_nucleo );

	puts("Terminé felizmente");

	return EXIT_SUCCESS;
}
