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

#include <stdlib.h>
#include <string.h>
#include <parser/metadata_program.h>

	typedef struct Buffer {
		void *data;
		int next;
		size_t size;
	} Buffer;

	typedef struct response{
		int32_t ok;
		int32_t codError;
		int32_t contenidoSize;
		char* contenido;
	} response;

	char* serializarResponse(response* response, int* responseSize);

	struct Buffer *new_buffer();

	//Reserva espacio dinámicamente y aumenta el tamaño del buffer bajo demanda
	void reserve_space(Buffer *buffer, size_t bytesNeeded);


	//Concatena string en buffer utilizando PRIMITIVE_SEPARATOR como separador
	void serialize_string(char* string, Buffer *buffer);
	void serialize_end_of_string(char* string);

	//Contatena un caracter especial de finalización de estructura
	void serialize_ending_special_character(char *specialCharacter, Buffer *buffer);

	//Concatena integer en buffer utilizando PRIMITIVE_SEPARATOR como separador
	void serialize_int(int integer, Buffer *buffer);
	void serialize_codeIndex(t_intructions* codeIndex, t_size instructionsCount, Buffer *buffer);

	t_intructions* deserialize_codeIndex(char* serializedCodeIndex, t_size instructionsCount);

	int convertToInt32(char* buffer);


#endif
