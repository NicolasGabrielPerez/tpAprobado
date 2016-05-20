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
#ifndef SERIALIZATION_H_
#define SERIALIZATION_H_
#include <commons/string.h>
#include <stdio.h>
#include <string.h>
#include "estructuras.h"

/**
* @NAME: serializarInt
* @DESC: serializado es la estructura donde se van copiando los bytes serializados
* devuelve cantidad de bytes serializados
*/
char* serializarInt(char* serializado, int32_t value);

/**
* @NAME: serializarString
* @DESC: serializado es la estructura donde se van copiando los bytes serializados
* devuelve cantidad de bytes serializados
* */
char* serializarString(char* serializado, char* value);

/**
* @NAME: serializarPCB
* @DESC: devuelve puntero a serializado
*/
char* serializarPCB(PCB* pcb);

/**
* @NAME: deserializarPCB
* @DESC: devuelve puntero a pcb deserializado
*/
PCB* deserializarPCB(char* serializado);

#endif /* SERIALIZATION */
