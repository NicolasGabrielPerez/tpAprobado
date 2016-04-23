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

int main(void) {
	t_config* config = config_create("consola.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}
	int puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
	int cantidad_de_paginas = config_get_int_value(config, "CANTIDAD_PAGINAS");
	int tamanio_de_pagina = config_get_int_value(config, "TAMANIO_PAGINA");
	int retardo_compactacion = config_get_int_value(config, "RETARDO_COMPACTACION");

	int sockfd, numbytes;
	char buf[50];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo("utnso40", "8989", &hints, &servinfo)) != 0) {
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

	if (p == NULL) {
	fprintf(stderr, "client: failed to connect\n");
	return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
		s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	puts("Núcleo: Voy a enviar algo...\n");
	if (send(sockfd,"Hola!", 5, 0) == -1) {
	  perror("send");
	}

    if ((numbytes = recv(sockfd, buf, 20, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    printf("numbytes: '%d'\n",numbytes);
	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);
	//printf("client: size of what received '%s'\n",string_length(buf));

	puts("Antes de close\n");
	close(sockfd);
	puts("Despues de close\n");

	return EXIT_SUCCESS;
}
