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
#ifndef NUCLEO_FUNCTIONS_H_
#define NUCLEO_FUNCTIONS_H_

#include <parser/parser.h>
#include <commons/config.h>

#include <sockets/pcb.h>

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

void nucleo_init(t_config* config);
void nucleo_delete();

PCB* nucleo_recibir_pcb();

void nucleo_notificarIO(t_nombre_dispositivo valor, u_int32_t tiempo);
void nucleo_notificarFinDeQuantum(u_int32_t quantumCount);
char* nucleo_notificarFinDeRafaga(PCB* pcb);
void nucleo_notificarFinDePrograma(PCB* pcb);

void nucleo_wait(t_nombre_semaforo semaforo); //Recibe valor semaforo
void nucleo_signal(t_nombre_semaforo semaforo); //Recive valor semaforo

void nucleo_imprimir(t_valor_variable valor);
void nucleo_imprimir_texto(char* texto);

t_valor_variable nucleo_variable_compartida_obtener(t_nombre_compartida variable);
void nucleo_variable_compartida_asignar(t_nombre_compartida variable, t_valor_variable valor);

#endif
