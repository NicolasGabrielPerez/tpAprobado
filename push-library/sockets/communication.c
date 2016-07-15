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
int32_t HEADER_INVALIDO = 900;

int32_t HEADER_HANDSHAKE = 100;
int32_t HEADER_INIT_PROGRAMA = 200;
int32_t HEADER_SOLICITAR_PAGINAS = 300;
int32_t HEADER_ALMACENAR_PAGINAS = 400;
int32_t HEADER_FIN_PROGRAMA = 600;
int32_t HEADER_CAMBIO_PROCESO_ACTIVO = 700;
int32_t HEADER_RESULTADOS = 800;
int32_t HEADER_PAGINAS_DISPONIBLES = 1101;
int32_t HEADER_PAGINAS_NO_DISPONIBLES = 1002;
int32_t HEADER_ENVIAR_PCB = 1003;
int32_t HEADER_NOTIFICAR_IO = 1004;
int32_t HEADER_NOTIFICAR_FIN_QUANTUM = 1005;
int32_t HEADER_NOTIFICAR_FIN_RAFAGA = 1006;
int32_t HEADER_NOTIFICAR_WAIT = 1007;
int32_t HEADER_NOTIFICAR_SIGNAL = 1008;
int32_t HEADER_IMPRIMIR = 1009;
int32_t HEADER_IMPRIMIR_TEXTO = 1010;
int32_t HEADER_WAIT_CONTINUAR = 1100;
int32_t HEADER_SETEAR_VARIABLE = 1102;
int32_t HEADER_OBTENER_VARIABLE = 1103;
int32_t SIGUSR1 = 9999;

int32_t HEADER_ENVIAR_QUANTUM = 2001;

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

	if (recv(socket, &respuesta->ok, sizeof(int32_t), 0) <=0 ) {
		free(respuesta);
		return createFAILResponse(SOCKET_DESCONECTADO);
	}
	if (recv(socket, &respuesta->codError, sizeof(int32_t), 0) <=0) {
		free(respuesta);
		return createFAILResponse(SOCKET_DESCONECTADO);
	}
	if (recv(socket, &respuesta->contenidoSize, sizeof(int32_t), 0) <=0) {
		free(respuesta);
		return createFAILResponse(SOCKET_DESCONECTADO);
	}
	if(respuesta->contenidoSize >0){
		respuesta->contenido = malloc(respuesta->contenidoSize);
		if (recv(socket, respuesta->contenido, respuesta->contenidoSize, 0) <=0) {
			free(respuesta);
			return createFAILResponse(SOCKET_DESCONECTADO);
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

//Creador de response con campo ok=1, el resto en 0
response* createOKResponse(){
	return createResponse(1,0,0,0);
}

//Creador de response con campo ok=0, codError=codError, el resto en 0
response* createFAILResponse(int codError){
	return createResponse(0,codError,0,0);;
}

int enviarResponse(int socket, response* respuesta){
	int respuestaSize;
	char* respuestaSerializada = serializarResponse(respuesta, &respuestaSize);
	ssize_t result = send(socket, respuestaSerializada, respuestaSize, 0);

	free(respuesta);
	free(respuestaSerializada);
	return result;
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

void enviarResultado(response* response, int socket){
	if(response->ok){
		enviarOKConContenido(socket, response->contenidoSize, response->contenido);
	} else{
		enviarFAIL(socket, response->codError);
	}
}

message* receiveMessage(int socket){
	message* message = malloc(sizeof(message));

	// HEADER
	response* responseHeader = recibirResponse(socket);
	if(!responseHeader->ok){
		message->codError = responseHeader->codError;
		deleteResponse(responseHeader);
		return message;
	}
	message->header = convertToInt32(responseHeader->contenido);
	deleteResponse(responseHeader);

	//PAYLOAD
	response* responsePayload = recibirResponse(socket);
	message->contenido = responsePayload->contenido;
	message->contenidoSize = responsePayload->contenidoSize;
	if(!responsePayload->ok){
		message->codError = responsePayload->codError;
		deleteResponse(responsePayload);
		return message;
	}

	return message;
}

int32_t sendMessage(int socket, int header, int contenidoSize, char* contenidoSerializado){
	//HEADER
	char* headerSerializado = malloc(sizeof(int32_t));
	int32_t headerSelf;
	memcpy(&headerSelf, &header, sizeof(int32_t));

	printf("Header a enviar: %d\n", headerSelf);

	memcpy(headerSerializado, &headerSelf, sizeof(int32_t));
	int headerResult = enviarOKConContenido(socket, sizeof(int32_t), headerSerializado);

	free(headerSerializado);

	if(headerResult == -1) {
		return headerResult;
	}

	//PAYLOAD
	int payloadResult = enviarOKConContenido(socket, contenidoSize, contenidoSerializado);
	return payloadResult;
}

int32_t sendMessageInt(int socket, int header, int value){
	//HEADER
	char* headerSerializado = malloc(sizeof(int32_t));
	int32_t headerSelf;
	memcpy(&headerSelf, &header, sizeof(int32_t));

	printf("Header a enviar: %d\n", headerSelf);

	memcpy(headerSerializado, &headerSelf, sizeof(int32_t));
	int headerResult = enviarOKConContenido(socket, sizeof(int32_t), headerSerializado);

	if(headerResult == -1){
		return headerResult;
	}

	int32_t payloadSelf;
	memcpy(&payloadSelf, &value, sizeof(int32_t));

	char* serializedValue = malloc(sizeof(int32_t));
	serializarInt(serializedValue, &value);

	//PAYLOAD
	return enviarOKConContenido(socket, sizeof(int32_t), serializedValue);
}

int32_t sendErrorMessage(int socket, int header, int errorCode){
	//HEADER
	char* headerSerializado = malloc(sizeof(int32_t));
	int32_t headerSelf;
	memcpy(&headerSelf, &header, sizeof(int32_t));

	printf("Header a enviar: %d\n", headerSelf);

	memcpy(headerSerializado, &headerSelf, sizeof(int32_t));
	return enviarFAIL(socket,errorCode);
}
