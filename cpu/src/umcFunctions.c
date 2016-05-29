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

#include "sockets.h"

int socket_umc;

const u_int32_t BUFFER_SIZE_UMC = 1024;
const u_int32_t HEADER_SIZE_UMC = sizeof(int32_t);

void umc_init(t_config* config){



	char* puerto_umc = config_get_string_value(config, "PUERTO_UMC");
	char* ip_umc = config_get_string_value(config, "IP_UMC");
	printf("Config: PUERTO_UMC=%s\n", puerto_umc);

	socket_umc = crear_socket_cliente(ip_umc, puerto_umc); //socket usado para conectarse a la umc
	printf("UMC FD: %d\n", socket_umc);

	//Hago handshake con umc
	if(handshake(socket_umc, "PRUEBA") != 0){
		puts("Error en handshake con la umc");
	}
}

void umc_delete() {
	close(socket_umc);
}

void umc_process_active(int32_t processId) {

	char* bufferHeader[HEADER_SIZE_UMC];
	memcpy(bufferHeader, &processId, sizeof(int32_t));

	int bytesHeader = HEADER_SIZE_UMC;
	if (send(socket_umc, bufferHeader, bytesHeader, 0) == -1) {
		 perror("Error enviando header proceso activo");
	};

	const int bytesPayload = sizeof(int32_t);
	char bufferPayload[bytesPayload];
	memcpy(bufferPayload, &processId, sizeof(int32_t));

	if (send(socket_umc, bufferPayload, bytesPayload, 0) == -1) {
		perror("Error enviando payload proceso activo");
	};
}

void umc_set(t_puntero page, t_puntero offset, u_int32_t size, char* buffer) {
	int nbytes = 10;
	if (send(socket_umc, buffer, nbytes, 0) == -1) {
		 perror("Error insertando memoria");
	};
}

char* umc_get(t_puntero page, t_puntero offset, u_int32_t size) {

	char buf[BUFFER_SIZE_UMC];
	int nbytes = 10;
	if (send(socket_umc, buf, nbytes, 0) == -1) {
		perror("Error obteniendo memoria");
	};

	//Quiero recibir de núcleo, lo que le pasó consola
	if ((nbytes = recv(socket_umc, buf, BUFFER_SIZE_UMC, 0)) <= 0) {
	   // got error or connection closed by client
	   if (nbytes == 0) {
		   // connection closed
		   printf("socket %d hung up\n", socket_umc);
	   } else {
		   perror("recv");
	   }
	   close(socket_umc); // bye!
	} else {
	   //se recibió mensaje
	   printf("Se recibieron %d bytes\n", nbytes);
	   printf("Se recibió: %s\n", buf);
	}

	char* result = buf;
	return result;
}
