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

#include "nucleoFunctions.h"
#include "cpu.h"

#include <sockets/sockets.h>
#include <sockets/communication.h>

int socket_nucleo;

int32_t HEADER_SIZE_NUCLEO = sizeof(int32_t);
int32_t BUFFER_SIZE_NUCLEO = 1024;


int32_t HEADER_NUCLEO_HANDSHAKE = 1;
int32_t HEADER_NUCLEO_INIT_PROGRAMA = 2;
int32_t HEADER_RECIBIR_PCB = 3;
int32_t HEADER_NOTIFICAR_IO = 4;
int32_t HEADER_NOTIFICAR_FIN_QUANTUM = 5;
int32_t HEADER_NOTIFICAR_FIN_PROGRAMA = 6;
int32_t HEADER_NOTIFICAR_FIN_RAFAGA = 7;
int32_t HEADER_NOTIFICAR_WAIT = 8;
int32_t HEADER_NOTIFICAR_SIGNAL = 9;
int32_t HEADER_IMPRIMIR = 10;
int32_t HEADER_IMPRIMIR_TEXTO = 11;
int32_t HEADER_VARIABLE_COMPARTIDA_OBTENER = 12;
int32_t HEADER_VARIABLE_COMPARTIDA_ASIGNAR = 13;



int receiveData(char* bufferResult) {
	//Devuelve la cantidad de bytes recibidos y un buffer

	message* mensaje;
	mensaje = receiveMessage(socket_nucleo);

	if(mensaje->codError != 0){
		log_error(logger, "recv");
	}

	if(mensaje->header == HEADER_ENVIAR_PCB){
		enviarPCB();
	}
	if(mensaje->header == HEADER_WAIT_CONTINUAR){
		continuarEjecucion();
	}

	if(mensaje->header == HEADER_OBTENER_VARIABLE){
		continuarEjecucion();//TODO no se q hacer
	}

	if(mensaje->header == SIGUSR1){
		desconectarse();
	}


	return 0;
}

continuarEjecucion(){
	//TODO DEsarrollar
}
void desconectarse(){
	//TODO: desconectarse luego de ejecutar la rafaga
}

void enviarPCB(){
	PCB* unPCB;//TODO  AGUS -variable global o algo asi

	Buffer *buffer = malloc(sizeof(Buffer));
	buffer = new_buffer();
	char* pcbzerial = serialize_pcb(unPCB ,buffer);
	int size = sizeof(pcbzerial);
	buffer_free( buffer);
	sendMessage(socket_nucleo, HEADER_ENVIAR_PCB,size , pcbzerial);

}

void nucleo_init(t_config* config) {

	//Hacer HANDSHAKE: HEADER_HANDSHAKE
	//Enviar tipo: TIPO_CPU

	char* puerto_nucleo = config_get_string_value(config, "PUERTO_NUCLEO");
	char* ip_nucleo = config_get_string_value(config, "IP_NUCLEO");
	log_trace(logger, "NUCLEO IP: %s PUERTO: %s\n", ip_nucleo, puerto_nucleo);

	socket_nucleo = crear_socket_cliente(ip_nucleo, puerto_nucleo); //socket usado para conectarse al nucleo

	//Hago handshake con umc
	if (sendMessage(socket_nucleo, HEADER_HANDSHAKE, 0, 0) == -1) {
			log_error(logger, "Error enviando handshake nucleo");
			exitProgram();
	};

//TODO: ver como mandar el tipo cpu
	// Envio mi tipo: CPUs
	char* bufferType = string_itoa(HEADER_SIZE_NUCLEO);
	int bytesType = sizeof(char) * (string_length(bufferType) + 1);
	if (send(socket_nucleo, bufferType, bytesType, 0) == -1) {
		log_error(logger, "Error enviando tipo a nucleo");
		exitProgram();
	};
	free(bufferType);

	log_trace(logger, "Iniciado socket: Nucleo");
}

void nucleo_delete(){//final
	close(socket_nucleo);
}



PCB* nucleo_recibir_pcb() {

	log_trace(logger, string_from_format("NUCLEO: recibiendo PCB"));

	int LENGTH = 1024;
	char* buffer = malloc(sizeof(char*) * LENGTH);
	receiveData(buffer);

	PCB* pcb = deserialize_pcb(buffer);

	free(buffer);

	return pcb;
}

void nucleo_notificarIO(t_nombre_dispositivo valor) {//final

	log_trace(logger, string_from_format("NUCLEO: notificar IO"));



	if (sendMessage(socket_nucleo, HEADER_NOTIFICAR_IO, sizeof(valor), valor) == -1) {
		log_error(logger, "Error enviando  IO");
	}
}

void nucleo_notificarFinDeQuantum(u_int32_t quantumCount) {//final

	log_trace(logger, string_from_format("NUCLEO: FIN QUANTUM, %d", quantumCount));

	if (sendMessageInt(socket_nucleo, HEADER_NOTIFICAR_FIN_QUANTUM, quantumCount) == -1) {
		log_error(logger, "Error enviando Fin de Quantum");
	};
}

void nucleo_notificarFinDePrograma() {//final


	log_trace(logger, string_from_format("NUCLEO: fin de programa"));


	if (sendMessage(socket_nucleo, HEADER_FIN_PROGRAMA,0,0) == -1) {
		log_error(logger, "Error enviando Fin de Programa");
	};
}

void nucleo_notificarFinDeRafaga() {//final

	log_trace(logger, string_from_format("NUCLEO: fin de rafaga"));


	//Enviar PCB
	if (sendMessage(socket_nucleo, HEADER_NOTIFICAR_FIN_RAFAGA,0,0) == -1) {
		 log_error(logger, "Error enviando Fin de Rafaga");
	};
}

void nucleo_wait(t_nombre_semaforo semaforo) {//final

	log_trace(logger, string_from_format("NUCLEO: wait, %s", semaforo));

	if (sendMessage(socket_nucleo, HEADER_NOTIFICAR_WAIT,sizeof(t_nombre_semaforo),semaforo) == -1) {
		 log_error(logger, "Error enviando Wait");
	};
}

void nucleo_signal(t_nombre_semaforo semaforo) {//final

	log_trace(logger, string_from_format("NUCLEO: signal, %s", semaforo));

	if (sendMessage(socket_nucleo, HEADER_NOTIFICAR_WAIT,sizeof(t_nombre_semaforo),semaforo) == -1) {
		 log_error(logger, "Error enviando Signal");
	};
}

void nucleo_imprimir(t_valor_variable valor) {//TODO

	log_trace(logger, string_from_format("NUCLEO: imprimir variable, %d", valor));




	if (send(socket_nucleo, &HEADER_IMPRIMIR, sizeof(int32_t), 0) == -1) {
		 log_error(logger, "Error enviando header Signal");
	};

	if (send(socket_nucleo, &valor, sizeof(t_valor_variable), 0) == -1) {
		 log_error(logger, "Error enviando nomnre Signal");
	};
}

void nucleo_imprimir_texto(char* texto) {//final

	log_trace(logger, string_from_format("NUCLEO: imprimir texto, %s", texto));

	if (sendMessage(socket_nucleo, HEADER_FIN_PROGRAMA, sizeof(char) * (string_length(texto) + 1),texto) == -1) {
		 log_error(logger, "Error enviando texto");
	};
}

//TODO estas dos

t_valor_variable nucleo_variable_compartida_obtener(t_nombre_compartida variable) {

	log_trace(logger, string_from_format("NUCLEO: obtener variable compartida, %s", variable));

	return 0;
}

void nucleo_variable_compartida_asignar(t_nombre_compartida variable, t_valor_variable valor){
	log_trace(logger, string_from_format("NUCLEO: asignar variable compartida, %s %d", variable, valor));
}

