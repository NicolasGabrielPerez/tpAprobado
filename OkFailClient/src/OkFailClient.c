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

char* serializarInt(char* posicionDeEscritura, int32_t* value){
	int inputSize = sizeof(int32_t);
	memcpy(posicionDeEscritura, value, inputSize);
	return posicionDeEscritura + inputSize;
}

char* serializarString(char* posicionDeEscritura, char* value, int size){
	memcpy(posicionDeEscritura, value, size);
	return posicionDeEscritura + size;
}

char* serializarResponse(response* response, int* responseSize){
	*responseSize = sizeof(int32_t)*3 + response->contenidoSize;
	char* respuestaSerializada = malloc(*responseSize);
	char* siguiente = respuestaSerializada;

	siguiente = serializarInt(siguiente, &response->ok);
	siguiente = serializarInt(siguiente, &response->codError);
	siguiente = serializarInt(siguiente, &response->contenidoSize);

	if(response->contenidoSize>0){
		siguiente = serializarString(siguiente, response->contenido, response->contenidoSize);
	}

	return respuestaSerializada;
}

response* createResponse(int ok, int codError, int contenidoSize, char* contenido){
	response* respuesta = malloc(sizeof(response));
	respuesta->ok = ok;
	respuesta->codError = codError;
	respuesta->contenidoSize = contenidoSize;
	respuesta->contenido = contenido;
	return respuesta;
}

int enviarResponse(int socket, response* respuesta){
	int respuestaSize;
	char* respuestaSerializada = serializarResponse(respuesta, &respuestaSize);
	if (send(socket, respuestaSerializada, respuestaSize, 0) == -1) {
		perror("recv");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int enviarOKSinContenido(int socket){
	response* respuesta = createResponse(1,0,0,0);
	return enviarResponse(socket, respuesta);
}

int enviarOKConContenido(int socket, int contenidoSize, char* contenido){
	response* respuesta = createResponse(1,0,contenidoSize,contenido);
	return enviarResponse(socket, respuesta);
}

int enviarFAIL(int socket, int codError){
	response* respuesta = createResponse(0,codError,0,0);
	return enviarResponse(socket, respuesta);
}

int main(void) {
	int socket = crear_socket_cliente("utnso40","8008");

	enviarOKSinContenido(socket);

	printf("Enviada primer response\n");

	char contenido[5] = "Hola";

	enviarOKConContenido(socket, 5,contenido);

	printf("Enviada segunda response\n");

	enviarFAIL(socket, 9);

	return EXIT_SUCCESS;
}
