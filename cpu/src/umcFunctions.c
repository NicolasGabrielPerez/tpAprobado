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


void umc_init(t_config* config){

	//Hacer HANDSHAKE: HEADER_HANDSHAKE
	//Enviar tipo: TIPO_CPU

	char* puerto_umc = config_get_string_value(config, "PUERTO_UMC");
	char* ip_umc = config_get_string_value(config, "IP_UMC");
	log_trace(logger, "UMC IP: %s PUERTO: %s\n", ip_umc, puerto_umc);

	socket_umc = crear_socket_cliente(ip_umc, puerto_umc); //socket usado para conectarse a la umc

	int32_t response = sendMessageInt(socket_umc, HEADER_HANDSHAKE, TIPO_CPU);
	if(response <= 0) {
		log_error(logger, "Se cerró la conexion");
	}

	message* message = receiveMessage(socket_umc);

	PAGE_SIZE = convertToInt32(message->contenido);

	log_trace(logger, "Tamaño de Página: %d", PAGE_SIZE);
}

void umc_delete() {
	close(socket_umc);
}

void umc_process_active(int32_t processId) {

	//Envio header: HEADER_CAMBIO_PROCESO_ACTIVO
	//Enviar processID
	//Recibir respuesta: RESPUESTA_OK, RESPUESTA_FAIL
	//En caso de fallo, hacer un receive adicional con un codigo int32.

	log_trace(logger, "UMC: Cambio proceso activo: %d", processId);

	int32_t resp = sendMessageInt(socket_umc, HEADER_CAMBIO_PROCESO_ACTIVO, 0);

	resp = sendMessageInt(socket_umc, HEADER_ALMACENAR_PAGINAS, processId);

	response* respuesta = recibirResponse(socket_umc);
}

void umc_set(t_puntero page, t_puntero offset, t_size size, char* buffer) {

	log_trace(logger, "UMC: set (Page: %d, Offset: %d, Size: %d, Buffer: %s)", page, offset, size, buffer);

	int32_t resp = sendMessageInt(socket_umc, HEADER_ALMACENAR_PAGINAS, 0);

	resp = sendMessageInt(socket_umc, HEADER_ALMACENAR_PAGINAS, page);

	resp = sendMessageInt(socket_umc, HEADER_ALMACENAR_PAGINAS, offset);

	resp = sendMessageInt(socket_umc, HEADER_ALMACENAR_PAGINAS, size);

	int length = string_length(buffer);
	sendMessage(socket_umc, HEADER_ALMACENAR_PAGINAS, sizeof(char) * length , buffer);


	//Recibir respuesta: RESPUESTA_OK, RESPUESTA_FAIL
	response* respuesta = recibirResponse(socket_umc);
	if(respuesta->ok == 0) {
		log_error(logger, "Error insertando memoria: RESPONSE");
	}

	free(respuesta);
	//En caso de fallo, hacer un receive adicional con un codigo int32.
}

char* umc_get(t_puntero page, t_puntero offset, t_size size) {

	log_trace(logger, "UMC: get (Page: %d, Offset: %d, Size: %d)", page, offset, size);

	int32_t resp = sendMessageInt(socket_umc, HEADER_SOLICITAR_PAGINAS, 0);

	resp = sendMessageInt(socket_umc, HEADER_SOLICITAR_PAGINAS, page);

	resp = sendMessageInt(socket_umc, HEADER_SOLICITAR_PAGINAS, offset);

	resp = sendMessageInt(socket_umc, HEADER_SOLICITAR_PAGINAS, size);

	//Recibir respuesta: RESPUESTA_OK, RESPUESTA_FAIL
	response* respuesta = recibirResponse(socket_umc);
	if(respuesta->ok == 0) {
		log_error(logger, "Error obteniendo memoria: RESPONSE");
	}
	//En caso de fallo, hacer un receive adicional con un codigo int32.

	char* result = respuesta->contenido;

	free(respuesta);

	return result;
}
