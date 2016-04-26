#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <commons/string.h>

#define MAXDATASIZE 100 // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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
	if (send(sockfd,"Soy una CPU", 5, 0) == -1) {
	  perror("send");
	}

	if ((numbytes = recv(sockfd, buf, 49, 0)) == -1) {
		perror("recv");
		exit(1);
	}

	printf("client: received '%s'\n",buf);
	printf("numbytes: '%d'\n",numbytes);
	buf[numbytes] = '\0';

	puts("Antes de close\n");
	close(sockfd);
	puts("Despues de close\n");

	return 0;
}

int main(void) {
	t_config* config = config_create("consola.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}
	char* puerto_nucleo = config_get_string_value(config, "PUERTO_NUCLEO");

	printf("Config: PUERTO_NUCLEO=%s\n", puerto_nucleo);

	int socket_nucleo = crear_socket_cliente("127.0.0.1", puerto_nucleo); //socket usado para conectarse a la umc

	//Hago handskae con umc
	if(handshake(socket_nucleo) != 0){
		puts("Error en handshake con el Núcleo");
	}

	return EXIT_SUCCESS;
}
