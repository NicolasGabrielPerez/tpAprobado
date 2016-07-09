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

#include "cpu.h"
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
	log_trace(logger, "UMC IP: %s PUERTO: %s\n", ip_umc, puerto_umc);

	socket_umc = crear_socket_cliente(ip_umc, puerto_umc); //socket usado para conectarse a la umc

	//Hago handshake con umc
	char* param = string_itoa(HEADER_HANDSHAKE);
	enviarOKConContenido(socket_umc, sizeof(char) * (string_length(param) + 1), param);
	free(param);

	// Envio mi tipo: CPUs
	param = string_itoa(TIPO_CPU);
	enviarOKConContenido(socket_umc, sizeof(char) * (string_length(param) + 1), param);
	free(param);

	response* respuesta = recibirResponse(socket_umc);
	if(respuesta->ok != RESPUESTA_OK) {
		log_error(logger, "Error recibiendo respuesta proceso activo");
		exitProgram();
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

	log_trace(logger, string_from_format("UMC: Cambio proceso activo: %d", processId));

	char bufferHeader[HEADER_SIZE_UMC];
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
		log_error(logger, "Error recibiendo respuesta proceso activo");
	}
}

void umc_set(t_puntero page, t_puntero offset, t_size size, char* buffer) {

	log_trace(logger, string_from_format("UMC: set (%d, %d, %d, %s", page, offset, size, buffer));

	//Envio header: HEADER_ALMACENAR_PAGINAS
	char* param = string_itoa(HEADER_ALMACENAR_PAGINAS);
	enviarOKConContenido(socket_umc, sizeof(char) * (string_length(param) + 1), param);
	free(param);

	//Enviar page
	param = string_itoa(page);
	enviarOKConContenido(socket_umc, sizeof(char) * (string_length(param) + 1), param);
	free(param);

	//Enviar offset
	param = string_itoa(offset);
	enviarOKConContenido(socket_umc, sizeof(char) * (string_length(param) + 1), param);
	free(param);

	//Enviar size
	param = string_itoa(size);
	enviarOKConContenido(socket_umc, sizeof(char) * (string_length(param) + 1), param);
	free(param);

	//Enviar buffer
	enviarOKConContenido(socket_umc, size, buffer);

	//Recibir respuesta: RESPUESTA_OK, RESPUESTA_FAIL
	response* respuesta = recibirResponse(socket_umc);
	if(respuesta->ok != RESPUESTA_OK) {
		log_error(logger, "Error insertando memoria: RESPONSE");
	}

	//En caso de fallo, hacer un receive adicional con un codigo int32.
}

t_valor_variable umc_get(t_puntero page, t_puntero offset, t_size size) {

	log_trace(logger, string_from_format("UMC: get (%d, %d, %d", page, offset, size));


	//Envio header: HEADER_SOLICITAR_PAGINAS
	char* param = string_itoa(HEADER_SOLICITAR_PAGINAS);
	enviarOKConContenido(socket_umc, sizeof(char) * (string_length(param) + 1), param);
	free(param);

	//Enviar page
	param = string_itoa(page);
	enviarOKConContenido(socket_umc, sizeof(char) * (string_length(param) + 1), param);
	free(param);

	//Enviar offset
	param = string_itoa(offset);
	enviarOKConContenido(socket_umc, sizeof(char) * (string_length(param) + 1), param);
	free(param);

	//Enviar size
	param = string_itoa(size);
	enviarOKConContenido(socket_umc, sizeof(char) * (string_length(param) + 1), param);
	free(param);

	//Recibir respuesta: RESPUESTA_OK, RESPUESTA_FAIL
	response* respuesta = recibirResponse(socket_umc);
	if(respuesta->ok != RESPUESTA_OK) {
		log_error(logger, "Error obteniendo memoria: RESPONSE");
	}
	//En caso de fallo, hacer un receive adicional con un codigo int32.

	t_valor_variable result = (u_int32_t) respuesta->contenido;

	return result;
}
