/*
 ============================================================================
 Name        : OkFailSending.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

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
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sockets/sockets.h>

typedef struct response{
	int32_t ok;
	int32_t codError;
	int32_t contenidoSize;
	char* contenido;
} response;

response* recibirResponse(int socket){
	response* respuesta = malloc(sizeof(response));
	if (recv(socket, &respuesta->ok, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}
	if (recv(socket, &respuesta->codError, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}
	if (recv(socket, &respuesta->contenidoSize, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}
	if(respuesta->contenidoSize >0){
		respuesta->contenido = malloc(respuesta->contenidoSize);
		if (recv(socket, respuesta->contenido, respuesta->contenidoSize, 0) == -1) {
			perror("recv");
			exit(1);
		}
	} else{
		respuesta->contenido = 0;
	}
	return respuesta;
}

void deleteResponse(response* response){
	if(response->contenidoSize >0){
		free(response->contenido);
	}
	free(response);
}

int main(void) {
	int listener = crear_puerto_escucha("8008");
	int socket_cliente = aceptarNuevaConexion(listener);

	response* respuestaOkSinContenido = recibirResponse(socket_cliente);

	printf("Ok: %d\n", respuestaOkSinContenido->ok);
	printf("CodError: %d\n", respuestaOkSinContenido->codError);
	printf("Contenido size: %d\n", respuestaOkSinContenido->contenidoSize);
	printf("Contenido: %s\n", respuestaOkSinContenido->contenido);

	deleteResponse(respuestaOkSinContenido);

	response* respuestaOkConContenido = recibirResponse(socket_cliente);

	printf("Ok: %d\n", respuestaOkConContenido->ok);
	printf("CodError: %d\n", respuestaOkConContenido->codError);
	printf("Contenido size: %d\n", respuestaOkConContenido->contenidoSize);
	printf("Contenido: %s\n", respuestaOkConContenido->contenido);

	deleteResponse(respuestaOkConContenido);

	response* respuestaFAIL = recibirResponse(socket_cliente);

	printf("Ok: %d\n", respuestaFAIL->ok);
	printf("CodError: %d\n", respuestaFAIL->codError);
	printf("Contenido size: %d\n", respuestaFAIL->contenidoSize);
	printf("Contenido: %s\n", respuestaFAIL->contenido);

	deleteResponse(respuestaFAIL);

	return EXIT_SUCCESS;
}
