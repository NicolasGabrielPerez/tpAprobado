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


#include "nucleoFunctions.h"

#include "sockets.h"

int socket_nucleo;

u_int32_t BUFFER_SIZE_NUCLEO = 1024;


int receiveData(char* bufferResult) {
	//Devuelve la cantidad de bytes recibidos y un buffer

	int nbytes = 0;
	if ((nbytes = recv(socket_nucleo, bufferResult, BUFFER_SIZE_NUCLEO, 0)) <= 0) {
	   // got error or connection closed by client
	   if (nbytes == 0) {
		   // connection closed
		   printf("socket %d hung up\n", socket_nucleo);
	   } else {
		   perror("recv");
	   }
	   close(socket_nucleo); // bye!
	} else {
	   //se recibió mensaje
	   printf("Se recibieron %d bytes\n", nbytes);
	   printf("Se recibió: %s\n", bufferResult);

	   return nbytes;
	}
	return 0;
}


void nucleo_init(t_config* config) {

	char* puerto_nucleo = config_get_string_value(config, "PUERTO_NUCLEO");
	printf("Config: PUERTO_NUCLEO=%s\n", puerto_nucleo);

	socket_nucleo = crear_socket_cliente("utnso40", puerto_nucleo); //socket usado para conectarse a la umc
	printf("NUCLEO FD: %d\n", socket_nucleo);

	//Hago handskae con nucleo
	if(handshake(socket_nucleo, "PRUEBA") != 0){
		puts("Error en handshake con la umc");
	}
}

void nucleo_delete(){
	close(socket_nucleo);
}


PCB* nucleo_recibirInstruccion() {

	char buffer[BUFFER_SIZE_NUCLEO];
	int bytesReceived = receiveData(buffer);

	PCB* pcb = init_pcb();
	return pcb;
}

void nucleo_notificarIO(t_nombre_dispositivo valor, u_int32_t tiempo) {
	char buf[BUFFER_SIZE_NUCLEO];
	int nbytes = 10;
	if (send(socket_nucleo, buf, nbytes, 0) == -1) {
		 perror("Error al notificar entrada/salida");
	};
}

void nucleo_notificarFinDeQuantum(int quantumCount) {
	char buf[BUFFER_SIZE_NUCLEO];
	int nbytes = 10;
	if (send(socket_nucleo, buf, nbytes, 0) == -1) {
		 perror("Error al notificar fin de quantum");
	};
}

void nucleo_notificarFinDePrograma(PCB* pcb) {
	char buf[BUFFER_SIZE_NUCLEO];
	int nbytes = 10;
	if (send(socket_nucleo, buf, nbytes, 0) == -1) {
		 perror("Error al notificar fin de programa");
	};
}

void nucleo_notificarFinDeRafaga(PCB* pcb) {
	char buf[BUFFER_SIZE_NUCLEO];
	int nbytes = 10;
	if (send(socket_nucleo, buf, nbytes, 0) == -1) {
		 perror("Error al notificar fin de rafaga");
	};
}

void nucleo_wait(t_nombre_semaforo semaforo) {
	char buf[BUFFER_SIZE_NUCLEO];
	int nbytes = 10;
	if (send(socket_nucleo, buf, nbytes, 0) == -1) {
		 perror("Error al notificar wait");
	};
}

void nucleo_signal(t_nombre_semaforo semaforo) {
	char buf[BUFFER_SIZE_NUCLEO];
	int nbytes = 10;
	if (send(socket_nucleo, buf, nbytes, 0) == -1) {
		 perror("Error al notificar signal");
	};
}
