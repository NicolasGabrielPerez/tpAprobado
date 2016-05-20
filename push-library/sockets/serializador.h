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
#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_
#include <commons/string.h>
#include <stdio.h>

/**
* @NAME: serializarInt
* @DESC: devuelve cantidad de bytes serializados
*/
int serializarInt(char** serializado, int32_t value);

/**
* @NAME: serializarString
* @DESC: devuelve cantidad de bytes serializados
* */
int serializarString(char** serializado, char* value);

#endif /* SERIALIZADOR_H_ */
