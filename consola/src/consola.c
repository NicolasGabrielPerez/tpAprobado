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
#include <pthread.h>

#define MAXDATASIZE 100 // max number of bytes we can get at once

pthread_t resultados;
pthread_t comando;
pthread_attr_t nucleo_attr;

int FEOP=1;



void espera_resultados(int socketCliente){

	int fin_program = 1;
	while(fin_program || FEOP){


		message* nucleoResponse;
		nucleoResponse = receiveMessage(socketCliente);
		// deserialize response


		if (nucleoResponse->header == HEADER_FIN_PROGRAMA) {
			printf("FIN DE PROGRAMA");
			fin_program = 0;
			FEOP = 0;
		}
		if (nucleoResponse->header == HEADER_PAGINAS_NO_DISPONIBLES) {
					printf("NO HAY MEMORIA DISPONIBLE");
					fin_program = 0;
					FEOP = 0;
				}
		if (nucleoResponse->header == HEADER_PAGINAS_DISPONIBLES) {
					printf("NO INICIADO");

				}
		if (nucleoResponse->header == HEADER_RESULTADOS){
			char* mensaje = malloc(nucleoResponse->contenidoSize);
			mensaje = (nucleoResponse->contenido);
			printf("%s", mensaje);
			free(mensaje);
		}

	}
}

void comandosPorPantalla(int socketCliente){
	char package[MAXDATASIZE];
	int enviar = 1;
	while(enviar || FEOP ){
		fgets(package, MAXDATASIZE, stdin);

		if (!strcmp(package,"kill")) {
			enviar = 0;
			sendMessage(socketCliente, HEADER_FIN_PROGRAMA, 0, "");
		}
	}

}


int main(int argc, char **argv) {
	pthread_attr_init(&nucleo_attr);
	pthread_attr_setdetachstate(&nucleo_attr, PTHREAD_CREATE_DETACHED);

	t_config* config = config_create("/home/utnso/consola.config");
	if(config==NULL){
		printf("No se pudo leer la configuración\n");
		return EXIT_FAILURE;
	}
	char* puerto_nucleo = config_get_string_value(config, "PUERTO_NUCLEO");

	printf("Config: PUERTO_NUCLEO=%s\n", puerto_nucleo);


	int socket_nucleo = crear_socket_cliente("utnso40", puerto_nucleo);

	//Hago handskae con umc
	if(sendMessage(socket_nucleo, HEADER_HANDSHAKE,0 , "") != 0){
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

	int header = HEADER_INIT_PROGRAMA;
	sendMessage(socket_nucleo, header, len, paquete);
	free(paquete);


	pthread_create(&resultados, &nucleo_attr, &espera_resultados, (void*) socket_nucleo);
	pthread_create(&comando, &nucleo_attr, &comandosPorPantalla, (void*) socket_nucleo);
	espera_resultados(socket_nucleo);




	puts("Terminé felizmente");

	return EXIT_SUCCESS;
}
