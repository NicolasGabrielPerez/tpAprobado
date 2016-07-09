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
#include <sys/socket.h>
#include <commons/string.h>

#include "umcFunctions.h"

#include <sockets/sockets.h>
#include <sockets/communication.h>


int socket_umc;

const u_int32_t BUFFER_SIZE_UMC = 1024;
const u_int32_t HEADER_SIZE_UMC = sizeof(int32_t);

//const u_int32_t PAGE_SIZE;

void umc_init(t_config* config){

	//Hacer HANDSHAKE: HEADER_HANDSHAKE
	//Enviar tipo: TIPO_CPU

	char* puerto_umc = config_get_string_value(config, "PUERTO_UMC");
	char* ip_umc = config_get_string_value(config, "IP_UMC");
	printf("Config: PUERTO_UMC=%s\n", puerto_umc);

	socket_umc = crear_socket_cliente(ip_umc, puerto_umc); //socket usado para conectarse a la umc
	printf("UMC FD: %d\n", socket_umc);

	//Hago handshake con umc
	char* bufferHandshake[HEADER_SIZE_UMC];
	memcpy(bufferHandshake, &HEADER_HANDSHAKE, sizeof(int32_t));

	enviarOKConContenido(socket_umc, sizeof(int32_t), bufferHandshake );

	free(bufferHandshake);

	// Envio mi tipo: CPUs
	char* bufferType[HEADER_SIZE_UMC];
	memcpy(bufferType, &TIPO_CPU, sizeof(int32_t));

	enviarOKConContenido(socket_umc, sizeof(int32_t), bufferType);

	free(bufferType);

	response* respuesta = recibirResponse(socket_umc);
	if(respuesta->ok != RESPUESTA_OK) {
		perror("Error recibiendo respuesta proceso activo");
	}

	PAGE_SIZE = (u_int32_t) respuesta->contenido;
}

void umc_delete() {
	close(socket_umc);
}

void umc_process_active(int32_t processId) {

	//Envio header: HEADER_CAMBIO_PROCESO_ACTIVO
	//Enviar processID
	//Recibir respuesta: RESPUESTA_OK, RESPUESTA_FAIL
	//En caso de fallo, hacer un receive adicional con un codigo int32.

	char* bufferHeader[HEADER_SIZE_UMC];
	memcpy(bufferHeader, &processId, sizeof(int32_t));

	int bytesHeader = HEADER_SIZE_UMC;

	enviarOKConContenido(socket_umc, bytesHeader, bufferHeader);

	free(bufferHeader);

	const int bytesPayload = sizeof(int32_t);
	char bufferPayload[bytesPayload];
	memcpy(bufferPayload, &processId, sizeof(int32_t));

	enviarOKConContenido(socket_umc, bytesPayload, bufferPayload);

	free(bufferPayload);

	response* respuesta = recibirResponse(socket_umc);
	if(respuesta->ok != RESPUESTA_OK) {
		perror("Error recibiendo respuesta proceso activo");
	}
}

void umc_set(t_puntero page, t_puntero offset, u_int32_t size, char* buffer) {

	//Envio header: HEADER_ALMACENAR_PAGINAS
	enviarOKConContenido(socket_umc, sizeof(int32_t), &HEADER_ALMACENAR_PAGINAS);

	//Enviar page
	enviarOKConContenido(socket_umc, sizeof(t_puntero), &page);

	//Enviar offset
	enviarOKConContenido(socket_umc, sizeof(t_puntero), &offset);

	//Enviar size
	enviarOKConContenido(socket_umc, sizeof(u_int32_t), &size);

	//Enviar buffer
	enviarOKConContenido(socket_umc, size, &buffer);

	//Recibir respuesta: RESPUESTA_OK, RESPUESTA_FAIL
	response* respuesta = recibirResponse(socket_umc);
	if(respuesta->ok != RESPUESTA_OK) {
		perror("Error insertando memoria: RESPONSE");
	}

	//En caso de fallo, hacer un receive adicional con un codigo int32.
}

t_valor_variable umc_get(t_puntero page, t_puntero offset, u_int32_t size) {

	//Envio header: HEADER_SOLICITAR_PAGINAS
	enviarOKConContenido(socket_umc, sizeof(int32_t), &HEADER_SOLICITAR_PAGINAS);

	//Enviar page
	enviarOKConContenido(socket_umc, sizeof(t_puntero), &page);

	//Enviar offset
	enviarOKConContenido(socket_umc, sizeof(t_puntero), &offset);

	//Enviar size
	enviarOKConContenido(socket_umc, sizeof(u_int32_t), &size);

	//Recibir respuesta: RESPUESTA_OK, RESPUESTA_FAIL
	response* respuesta = recibirResponse(socket_umc);
	if(respuesta->ok != RESPUESTA_OK) {
		perror("Error obteniendo memoria: RESPONSE");
	}
	//En caso de fallo, hacer un receive adicional con un codigo int32.

	t_valor_variable result = (u_int32_t) respuesta->contenido;

	return result;
}
