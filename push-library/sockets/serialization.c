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

#include "serialization.h"

#define INITIAL_SIZE 32

struct Buffer *new_buffer() {
    struct Buffer *b = malloc(sizeof(Buffer));

    b->data = malloc(INITIAL_SIZE);
    b->size = INITIAL_SIZE;
    b->next = 0;

    return b;
}

void reserve_space(Buffer *b, size_t bytes) {
    if((b->next + bytes) > b->size) {
        /* double size to enforce O(lg N) reallocs */
        b->data = realloc(b->data, b->size * 2);
        b->size *= 2;
    }
}

char* serializar_Int(char* posicionDeEscritura, int32_t* value){
	int inputSize = sizeof(int32_t);
	memcpy(posicionDeEscritura, value, inputSize);
	return posicionDeEscritura + inputSize;
}

char* serializar_String(char* posicionDeEscritura, int size, char* value){
	memcpy(posicionDeEscritura, value, size);
	return posicionDeEscritura + size;
}

char* serializarResponse(response* response, int* responseSize){
	*responseSize = sizeof(int32_t)*3 + response->contenidoSize;
	char* respuestaSerializada = malloc(*responseSize);
	char* siguiente = respuestaSerializada;

	siguiente = serializar_Int(siguiente, &response->ok);
	siguiente = serializar_Int(siguiente, &response->codError);
	siguiente = serializar_Int(siguiente, &response->contenidoSize);

	if(response->contenidoSize>0){
		siguiente = serializar_String(siguiente, response->contenidoSize, response->contenido);
	}

	return respuestaSerializada;
}

int convertToInt32(char* buffer){
	int32_t* number = malloc(sizeof(int32_t));
	memcpy(number, buffer, sizeof(int32_t));
	return *number;
}
