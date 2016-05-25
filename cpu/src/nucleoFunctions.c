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

#include "nucleoFunctions.h"

int socket_nucleo;

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

}

void nucleo_notificarFinDeQuantum(int quantumCount) {

}

void nucleo_notificarFinDePrograma(PCB* pcb) {

}

void nucleo_notificarFinDeRafaga(PCB* pcb) {

}

void nucleo_wait(t_nombre_semaforo semaforo) {

}

void nucleo_signal(t_nombre_semaforo semaforo) {

}
