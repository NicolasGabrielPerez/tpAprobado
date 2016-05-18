#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <parser/metadata_program.h>
#include <parser/parser.h>
#include <parser/sintax.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <sockets/sockets.h>

#include "ansiop.h"

static const char* DEFINICION_VARIABLES = "variables a, b, c";
static const char* ASIGNACION = "a = b + 12";
static const char* IMPRIMIR = "print b";
static const char* IMPRIMIR_TEXTO = "textPrint foo\n";

int socketNucleo = 0;
int socketUmc = 0;

AnSISOP_funciones functions = {
	.AnSISOP_definirVariable	= definirVariable,
	.AnSISOP_obtenerPosicionVariable= obtenerPosicionVariable,
	.AnSISOP_dereferenciar	= dereferenciar,
	.AnSISOP_asignar	= asignar,
	.AnSISOP_imprimir	= imprimir,
	.AnSISOP_imprimirTexto	= imprimirTexto,
};
AnSISOP_kernel kernel_functions = { };

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void correrDefinirVariables() {
	printf("Ejecutando '%s'\n", DEFINICION_VARIABLES);
	analizadorLinea(strdup(DEFINICION_VARIABLES), &functions, &kernel_functions);
	printf("================\n");
}


void correrAsignar() {
	printf("Ejecutando '%s'\n", ASIGNACION);
	analizadorLinea(strdup(ASIGNACION), &functions, &kernel_functions);
	printf("================\n");
}


void correrImprimir() {
	printf("Ejecutando '%s'\n", IMPRIMIR);
	analizadorLinea(strdup(IMPRIMIR), &functions, &kernel_functions);
	printf("================\n");
}



void correrImprimirTexto() {
	printf("Ejecutando '%s'", IMPRIMIR_TEXTO);
	analizadorLinea(strdup(IMPRIMIR_TEXTO), &functions, &kernel_functions);
	printf("================\n");
}

int main(int argc, char **argv) {
	//correrDefinirVariables();
	//correrAsignar();
	//correrImprimir();
	//correrImprimirTexto();
	//initConfig();

//	sendMessage();

	char buf[50];
	int nbytes;

	t_config* config = config_create("cpu.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}

	char* puerto_nucleo = config_get_string_value(config, "PUERTO_NUCLEO");
	char* puerto_umc = config_get_string_value(config, "PUERTO_UMC");

	printf("Config: PUERTO_NUCLEO=%s\n", puerto_nucleo);
	printf("Config: PUERTO_UMC=%s\n", puerto_umc);

	int socket_umc = crear_socket_cliente("utnso40", puerto_umc); //socket usado para conectarse a la umc
	//int socket_nucleo = crear_socket_cliente("utnso40", puerto_nucleo); //socket usado para conectarse a la umc

	printf("UMC FD: %d\n", socket_umc);
	//printf("NUCLEO FD: %d\n", socket_nucleo);

	//Hago handshake con umc
	handshake(socket_umc, "Soy cpu");
//	if(handshake(socket_umc, "PRUEBA") != 0){
//		puts("Error en handshake con la umc");
//	}

	//Hago handskae con nucleo
//	if(handshake(socket_nucleo, "PRUEBA") != 0){
//		puts("Error en handshake con la umc");
//	}

	//Quiero recibir de núcleo, lo que le pasó consola
//	if ((nbytes = recv(socket_nucleo, buf, 50, 0)) <= 0) {
//	   // got error or connection closed by client
//	   if (nbytes == 0) {
//		   // connection closed
//		   printf("socket %d hung up\n", socket_nucleo);
//	   } else {
//		   perror("recv");
//	   }
//	   close(socket_nucleo); // bye!
//   } else {
//	   //se recibió mensaje
//	   printf("Se recibieron %d bytes\n", nbytes);
//	   printf("Se recibió: %s\n", buf);
//
//  } // END handle data from client
//
//
//	if (send(socket_umc, buf, nbytes, 0) == -1) { //envio lo mismo que me acaba de llegar => misma cant de bytes a enviar
//		 perror("send");
//    };
//
//	puts("Ya le hice un envio a UMC. Termino mi ejecucion.");

	return EXIT_SUCCESS;

}
