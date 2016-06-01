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

#include "communication.h"

int32_t HEADER_SIZE = sizeof(int32_t);
int32_t RESPUESTA_SIZE = sizeof(int32_t);
int32_t RESPUESTA_OK = 10;
int32_t RESPUESTA_FAIL = -10;

int32_t HEADER_HANDSHAKE = 100;
int32_t HEADER_INIT_PROGRAMA = 200;
int32_t HEADER_SOLICITAR_PAGINAS = 300;
int32_t HEADER_ALMACENAR_PAGINAS = 400;
int32_t HEADER_FIN_PROGRAMA = 600;
int32_t HEADER_CAMBIO_PROCESO_ACTIVO = 700;

int32_t TIPO_CONSOLA = 1000;
int32_t TIPO_NUCLEO = 2000;
int32_t TIPO_UMC = 3000;
int32_t TIPO_SWAP = 4000;
int32_t TIPO_CPU = 5000;
int32_t TIPO_SIZE = sizeof(int32_t);

char* serializarInt(char* posicionDeEscritura, int32_t* value){
	int inputSize = sizeof(int32_t);
	memcpy(posicionDeEscritura, value, inputSize);
	return posicionDeEscritura + inputSize;
}

char* serializarString(char* posicionDeEscritura, char* value){
	int inputSize = strlen(value) + 1;
	memcpy(posicionDeEscritura, value, inputSize);
	return posicionDeEscritura + inputSize;
}

void deleteResponse(response* response){
	if(response->contenidoSize >0){
		free(response->contenido);
	}
	free(response);
}

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
