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

#include "push-library/sockets.h"
#include "push-library/communication.h"

int socket_nucleo;

int32_t HEADER_NUCLEO_HANDSHAKE = 1;
int32_t HEADER_NUCLEO_INIT_PROGRAMA = 2;
int32_t HEADER_RECIBIR_PCB = 3;
int32_t HEADER_NOTIFICAR_IO = 4;
int32_t HEADER_NOTIFICAR_FIN_QUANTUM = 5;
int32_t HEADER_NOTIFICAR_FIN_PROGRAMA = 6;
int32_t HEADER_NOTIFICAR_FIN_RAFAGA = 7;
int32_t HEADER_NOTIFICAR_WAIT = 8;
int32_t HEADER_NOTIFICAR_SIGNAL = 9;

int32_t HEADER_SIZE_NUCLEO = sizeof(int32_t);
int32_t BUFFER_SIZE_NUCLEO = 1024;

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

	//Hacer HANDSHAKE: HEADER_HANDSHAKE
	//Enviar tipo: TIPO_CPU

	char* puerto_nucleo = config_get_string_value(config, "PUERTO_NUCLEO");
	char* ip_nucleo = config_get_string_value(config, "IP_NUCLEO");
	printf("Config: PUERTO_NUCLEO=%s\n", puerto_nucleo);

	socket_nucleo = crear_socket_cliente(ip_nucleo, puerto_nucleo); //socket usado para conectarse al nucleo
	printf("NUCLEO FD: %d\n", socket_nucleo);

	//Hago handshake con umc
	char* bufferHandshake[HEADER_SIZE_NUCLEO];
	memcpy(bufferHandshake, HEADER_NUCLEO_HANDSHAKE, sizeof(int32_t));
	int bytesHandshake = HEADER_SIZE_NUCLEO;
	if (send(socket_nucleo, bufferHandshake, bytesHandshake, 0) == -1) {
			perror("Error enviando handshake nucleo");
	};

	// Envio mi tipo: CPUs
	char* bufferType[HEADER_SIZE_NUCLEO];
	memcpy(bufferType, TIPO_CPU, sizeof(int32_t));
	int bytesType = HEADER_SIZE_NUCLEO;
	if (send(socket_nucleo, bufferType, bytesType, 0) == -1) {
		perror("Error enviando tipo a nucleo");
	};
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

	if (send(socket_nucleo, HEADER_NOTIFICAR_IO, sizeof(int32_t), 0) == -1) {
		 perror("Error enviando header IO");
	};

	if (send(socket_nucleo, tiempo, sizeof(u_int32_t), 0) == -1) {
		 perror("Error enviando tiempo IO");
	};
}

void nucleo_notificarFinDeQuantum(u_int32_t quantumCount) {
	if (send(socket_nucleo, HEADER_NOTIFICAR_FIN_QUANTUM, sizeof(int32_t), 0) == -1) {
		 perror("Error enviando header Fin de Quantum");
	};

	if (send(socket_nucleo, quantumCount, sizeof(u_int32_t), 0) == -1) {
		 perror("Error enviando count Fin de Quantum");
	};
}

void nucleo_notificarFinDePrograma(PCB* pcb) {
	if (send(socket_nucleo, HEADER_NOTIFICAR_FIN_PROGRAMA, sizeof(int32_t), 0) == -1) {
		 perror("Error enviando header Fin de Programa");
	};

	//Enviar PCB
//	if (send(socket_nucleo, quantumCount, sizeof(u_int32_t), 0) == -1) {
//		 perror("Error enviando count Fin de Quantum");
//	};
}

void nucleo_notificarFinDeRafaga(PCB* pcb) {
	if (send(socket_nucleo, HEADER_NOTIFICAR_FIN_RAFAGA, sizeof(int32_t), 0) == -1) {
		 perror("Error enviando header Fin de Rafaga");
	};

	//Enviar PCB
//	if (send(socket_nucleo, quantumCount, sizeof(u_int32_t), 0) == -1) {
//		 perror("Error enviando count Fin de Rafaga");
//	};
}

void nucleo_wait(t_nombre_semaforo semaforo) {
	if (send(socket_nucleo, HEADER_NOTIFICAR_WAIT, sizeof(int32_t), 0) == -1) {
		 perror("Error enviando header Wait");
	};

	if (send(socket_nucleo, semaforo, sizeof(t_nombre_semaforo), 0) == -1) {
		 perror("Error enviando nombre Wait");
	};
}

void nucleo_signal(t_nombre_semaforo semaforo) {
	if (send(socket_nucleo, HEADER_NOTIFICAR_SIGNAL, sizeof(int32_t), 0) == -1) {
		 perror("Error enviando header Signal");
	};

	if (send(socket_nucleo, semaforo, sizeof(t_nombre_semaforo), 0) == -1) {
		 perror("Error enviando nomnre Signal");
	};
}
