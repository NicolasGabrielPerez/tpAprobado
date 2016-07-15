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
#include "ansiop.h"
#include "cpu.h"

#include <sockets/sockets.h>
#include <sockets/communication.h>

int socket_nucleo;

void enviarPCB(){

	Buffer *buffer = new_buffer();
	char* pcbzerial = serialize_pcb(pcb, buffer);

	int size = sizeof(pcbzerial);
	buffer_free( buffer);
	sendMessage(socket_nucleo, HEADER_ENVIAR_PCB, size , pcbzerial);

	buffer_free(buffer);

	free_pcb(pcb);
	pcb = 0;
}

void nucleo_init(t_config* config) {

	//Hacer HANDSHAKE: HEADER_HANDSHAKE
	char* puerto_nucleo = config_get_string_value(config, "PUERTO_NUCLEO");
	char* ip_nucleo = config_get_string_value(config, "IP_NUCLEO");
	log_trace(logger, "NUCLEO IP: %s PUERTO: %s\n", ip_nucleo, puerto_nucleo);

	socket_nucleo = crear_socket_cliente(ip_nucleo, puerto_nucleo); //socket usado para conectarse al nucleo

	//Hago handshake con nucleo
//	if (sendMessageInt(socket_nucleo, HEADER_HANDSHAKE, TIPO_CPU) == -1) {
//		log_error(logger, "Error enviando handshake nucleo");
//		exitProgram();
//	}

	//Recibir el header
	message* message = receiveMessage(socket_nucleo);
	if(message->header == HEADER_HANDSHAKE) {
		log_trace(logger, "Iniciado socket: Nucleo");
	} else {
		log_error(logger, "Error al iniciar socket: Nucleo");
	}
}

void nucleo_delete(){//final
	close(socket_nucleo);
}



PCB* nucleo_recibir_pcb() {

	log_trace(logger, "NUCLEO: recibiendo PCB");

	message* message = receiveMessage(socket_nucleo);
	PCB* pcb = deserialize_pcb(message->contenido);

	return pcb;
}

void nucleo_notificarIO(t_nombre_dispositivo valor) {//final

	log_trace(logger, "NUCLEO: notificar IO");


	//Campo contenido mensaje el nombre del dispositivo.
	if (sendMessage(socket_nucleo, HEADER_NOTIFICAR_IO, sizeof(valor), valor) == -1) {
		log_error(logger, "Error enviando  IO");
	}

	//Enviar PCB
	enviarPCB(pcb);
	pcb = 0;
}

void nucleo_notificarFinDeQuantum(u_int32_t quantumCount) {

	log_trace(logger, "NUCLEO: FIN QUANTUM, %d", quantumCount);

	if (sendMessageInt(socket_nucleo, HEADER_NOTIFICAR_FIN_QUANTUM, quantumCount) == -1) {
		log_error(logger, "Error enviando Fin de Quantum");
	}
}

void nucleo_notificarFinDePrograma() {
	log_trace(logger, "NUCLEO: fin de programa");


	if (sendMessage(socket_nucleo, HEADER_FIN_PROGRAMA,0,0) == -1) {
		log_error(logger, "Error enviando Fin de Programa");
	}

	//No enviar PCB
}

char* nucleo_notificarFinDeRafaga() {//final

	log_trace(logger, "NUCLEO: fin de rafaga");

	if (sendMessage(socket_nucleo, HEADER_NOTIFICAR_FIN_RAFAGA,0,0) == -1) {
		 log_error(logger, "Error enviando Fin de Rafaga");
	}

	//Enviar PCB
	enviarPCB(pcb);
	pcb = 0;

	//TODO Ver retorno de notificarFinDeRafaga
	//Recibir respuesta de sisgur o no.

	return "test";
}

void nucleo_wait(t_nombre_semaforo semaforo) {//final

	log_trace(logger, "NUCLEO: wait, %s", semaforo);

	if (sendMessage(socket_nucleo, HEADER_NOTIFICAR_WAIT,sizeof(t_nombre_semaforo),semaforo) == -1) {
		 log_error(logger, "Error enviando Wait");
	}


	//Recibir mensaje: Si es WAIT_CONTINUAR sigo la ejecucion.
	//Si no es WAIT_CONTINUAR, envio PCB y dejo el program counter en el wait.

	message* message = receiveMessage(socket_nucleo);
	if(message->header == HEADER_WAIT_CONTINUAR) {
		//Continuar ejecucion
	} else {
		enviarPCB(pcb);
	}
}

void nucleo_signal(t_nombre_semaforo semaforo) {

	log_trace(logger, "NUCLEO: signal, %s", semaforo);

	if (sendMessage(socket_nucleo, HEADER_NOTIFICAR_SIGNAL, sizeof(t_nombre_semaforo),semaforo) == -1) {
		 log_error(logger, "Error enviando Signal");
	}
}

void nucleo_imprimir(t_valor_variable valor) {

	log_trace(logger, "NUCLEO: imprimir variable, %d", valor);


	char* textToPrint = string_itoa(valor);
	nucleo_imprimir_texto(textToPrint);

	free(textToPrint);
}

void nucleo_imprimir_texto(char* texto) {

	log_trace(logger, "NUCLEO: imprimir texto, %s", texto);

	if (sendMessage(socket_nucleo, HEADER_IMPRIMIR_TEXTO, sizeof(char) * (string_length(texto) + 1),texto) == -1) {
		 log_error(logger, "Error enviando texto");
	}
}

t_valor_variable nucleo_variable_compartida_obtener(t_nombre_compartida variable) {

	log_trace(logger, "NUCLEO: obtener variable compartida, %s", variable);

	//Enviar el nombre de la variable.
	//Recibir el valor.

	if (sendMessage(socket_nucleo, HEADER_OBTENER_VARIABLE, sizeof(t_nombre_compartida), variable) == -1) {
		log_error(logger, "Error obteniendo variable compartida");
	}

	message* message = receiveMessage(socket_nucleo);
	t_valor_variable valor = atoi(message->contenido);

	return valor;
}

void nucleo_variable_compartida_asignar(t_nombre_compartida variable, t_valor_variable valor){
	log_trace(logger, "NUCLEO: asignar variable compartida, %s %d", variable, valor);

	//usar globalVar en serialization.h para enviar la variable y el valor.
	//Serializar

	Buffer* buffer = new_buffer();
	t_globalVar* var = malloc(sizeof(t_globalVar));
	var->varName = variable;
	var->value = valor;

	char* data = serialize_globalVar(var, buffer);

	if (sendMessage(socket_nucleo, HEADER_SETEAR_VARIABLE, sizeof(char) * string_length(data), data) == -1) {
		 log_error(logger, "Error enviando variable compartida");
	}

	buffer_free(buffer);
	free(var);
}

