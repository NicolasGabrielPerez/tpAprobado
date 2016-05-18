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
#ifndef SOCKETS_H_
#define SOCKETS_H_

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

	/**
	* @NAME: temporal_get_string_time
	* @DESC: get sockaddr, IPv4 or IPv6
	*/

void *get_in_addr(struct sockaddr *sa);

	/**
	* @NAME: crear_socket_cliente
	* @DESC: conecta con esa ip y puerto, y devuelve el socket
	*/
int crear_socket_cliente(char* ip, char* port);

	/**
	* @NAME: handshake
	* @DESC: envía mensaje y recibe mensaje
	* 		devuelve el mensaje recibido
	* @PARAM:
	* 	send_message: mensaje a enviar
	*/
char* handshake(int sockfd, char* send_messaage);

	/**
	* @NAME: crear_puerto_escucha
	* @DESC: crea y devuelve puerto escucha
	*/
int crear_puerto_escucha(char* port);

#endif /* SOCKETS_H_ */
