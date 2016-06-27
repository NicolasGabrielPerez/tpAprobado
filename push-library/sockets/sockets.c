/*
 * Copyright (C) 2012 Sistemas Operativos - UTN FRBA. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
#include <arpa/inet.h>
#include "sockets.h"

#define HANDSHAKE_MESSAGE_SIZE 50;

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

char* handshake(int sockfd, char* send_messaage){
	int message_size = HANDSHAKE_MESSAGE_SIZE;
	char buf[message_size];
	int numbytes; //lo uso para poner la cantidad de bytes recibidos
	puts("Voy a enviar algo...\n");
	if (send(sockfd,send_messaage, message_size, 0) == -1) {
	  perror("send");
	}

	if ((numbytes = recv(sockfd, buf, message_size, 0)) == -1) {
		perror("recv");
		exit(1);
	}

	printf("received '%s'\n",buf);
	printf("numbytes: '%d'\n",numbytes);
	buf[numbytes] = '\0';

	puts("Handshake finalizado felizmente\n");

	return buf;
}

int crear_puerto_escucha(char* port){
	int listener;
	int rv;
	struct addrinfo hints, *ai, *p;
	int yes = 1;
	// get us a socket and bind it
   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;
   if ((rv = getaddrinfo(NULL, port, &hints, &ai)) != 0) {
	   fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
	   exit(1);
   }

   for(p = ai; p != NULL; p = p->ai_next) {
	   listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
	   if (listener < 0) {
		   continue;
	   }

	   // lose the pesky "address already in use" error message
	   setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	   if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
		   close(listener);
		   continue;
	   }

	   break;
   }

   // if we got here, it means we didn't get bound
   if (p == NULL) {
	   fprintf(stderr, "selectserver: failed to bind\n");
	   exit(2);
   }

   freeaddrinfo(ai); // all done with this

   // listen
   if (listen(listener, 10) == -1) {
	   perror("listen");
	   exit(3);
   }

   return listener;
}

// nuevo para enviar y recivir estructuras dinamicas

void send_dinamic(int sockfd, void* estructura, double tamanioEstructura){

	int tamanio_dato_double;

	if (send(sockfd, estructura, tamanio_dato_double, 0) == -1) {
		  perror("send");
	}
	if (send(sockfd,estructura, tamanioEstructura, 0) == -1) {
		  perror("send");
	}
}

void recv_dinamic(int sockfd, double tamanioDouble, int* bufer){
	int numbytes; //lo uso para poner la cantidad de bytes recibidos



	if ((tamanioDouble = recv(sockfd, numbytes, tamanioDouble, 0)) == -1) {
				perror("recv");
				exit(1);
		}
	bufer = malloc(tamanioDouble);
	if ((numbytes = recv(sockfd, bufer, numbytes, 0)) == -1) {
			perror("recv");
			exit(1);
	}


}

int aceptarNuevaConexion(int listener){
	int new_socket;
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;
	char remoteIP[INET6_ADDRSTRLEN];
	// handle new connections
	addrlen = sizeof remoteaddr;
	new_socket = accept(listener,
			(struct sockaddr *)&remoteaddr,
			&addrlen);
	puts("Conexion aceptada");
	if (new_socket == -1) {
		perror("accept");
	}
	printf("umc: new connection from %s on "
	"socket %d\n",
	inet_ntop(remoteaddr.ss_family,
	get_in_addr((struct sockaddr*)&remoteaddr),
			remoteIP, INET6_ADDRSTRLEN),
			listener);
	return new_socket;
}
