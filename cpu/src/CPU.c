/*
 ============================================================================
 Name        : CPU.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

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

#include "ansiop.h"

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

//void correrDefinirVariables() {
//	printf("Ejecutando '%s'\n", DEFINICION_VARIABLES);
//	analizadorLinea(strdup(DEFINICION_VARIABLES), &functions, &kernel_functions);
//}

int crear_socket_cliente(char* ip, char* port) { //devuelve un nuevo socket para conectarse al server especificado
	int sockfd; //aca se va a poner el socket obtenido mediante getaddrinfo

	struct addrinfo hints; //estructura conf info necesaria para getaddrinfo
	struct addrinfo *servinfo; //aca se va a poner la info del server
	struct addrinfo *p; //puntero usado para loopear los
	char s[INET6_ADDRSTRLEN]; //esto es para el nombre del server al que nos conectamos
	int rv; //valor que se usa para obtener especificacion de error, en caso de haberlo en getaddrinfo

	memset(&hints, 0, sizeof hints); //se asegura de 'limpiar' la memoria
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((getaddrinfo(ip, port, &hints, &servinfo)) != 0) {
	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) { //hubo error...
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	//se obtiene y se muestra el nombre del server
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
		s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	return sockfd;
}

int handshake(int sockfd){
	char buf[50];
	int numbytes; //lo uso para poner la cantidad de bytes recibidos
	puts("CPU: Voy a enviar algo...\n");
	if (send(sockfd,"Soy CPU    ", 11, 0) == -1) {
	  perror("send");
	}

	if ((numbytes = recv(sockfd, buf, 49, 0)) == -1) {
		perror("recv");
		exit(1);
	}

	printf("client: received '%s'\n",buf);
	printf("numbytes: '%d'\n",numbytes);
	buf[numbytes] = '\0';

	close(sockfd);
	puts("Swap: handshake finalizado felizmente\n");

	return 0;
}

int main(int argc, char **argv) {

	t_config* config = config_create("cpu.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}

	char* ip_nucleo = config_get_string_value(config, "IP_NUCLEO");
	char* ip_umc = config_get_string_value(config, "IP_UMC");

	char* puerto_nucleo = config_get_string_value(config, "PUERTO_NUCLEO");
	char* puerto_umc = config_get_string_value(config, "PUERTO_UMC");

	printf("Config: PUERTO_NUCLEO=%s\n", puerto_nucleo);
	printf("Config: PUERTO_UMC=%s\n", puerto_umc);

	int socket_umc = crear_socket_cliente(ip_umc, puerto_umc); //socket usado para conectarse a la umc
	int socket_nucleo = crear_socket_cliente(ip_nucleo, puerto_nucleo); //socket usado para conectarse a la umc

	//Hago handskae con umc
	if(handshake(socket_umc) != 0){
		puts("Error en handshake con la umc");
	}

	//Hago handskae con nucleo
	if(handshake(socket_nucleo) != 0){
		puts("Error en handshake con la umc");
	}
	// mensajes para checkpoint
    //conexiones checkpoint
          int PACKAGESIZE = 500;
          int status = 1;
          	char package[PACKAGESIZE];

          	while(status){
          			status = recv(socket_nucleo, (void*) package, PACKAGESIZE, 0);//cambiar socket al de nuclep
          			if (status) send(socket_umc, package, strlen(package) + 1, 0);//cambiar socket al de umc
          			if (status != 0) printf("%s", package);
          	}

          	close(socket_nucleo);
          	close(socket_umc);

	return EXIT_SUCCESS;
}
