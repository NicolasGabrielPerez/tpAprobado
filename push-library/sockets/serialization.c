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
#include <parser/metadata_program.h>
#include "pcb.h"

#define INITIAL_SIZE 32
#define PRIMITIVE_SEPARATOR "!"
#define CODEINDEX_SEPARATOR "#"
#define PCBSTRUCT_SEPARATOR "$"

struct Buffer *new_buffer() {
    struct Buffer *b = malloc(sizeof(Buffer));

    b->data = malloc(INITIAL_SIZE);
    b->size = INITIAL_SIZE;
    b->next = 0;

    return b;
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

//Reserva espacio dinámicamente y aumenta el tamaño del buffer bajo demanda
void reserve_space(Buffer *buffer, size_t bytesNeeded) {
    if((buffer->next + bytesNeeded) > buffer->size) {
    	buffer->data = realloc(buffer->data, buffer->size + bytesNeeded);
    	buffer->size += bytesNeeded;
    }
}

//Concatena string en buffer utilizando PRIMITIVE_SEPARATOR como separador
void serialize_string(char* string, Buffer *buffer) {

	char *cadena = string_new();
	string_append(&cadena, string);
	int stringSize = strlen(cadena);

    reserve_space(buffer, stringSize);

    //memcpy(((char *)buffer->data) + buffer->next, string, stringSize);
    memcpy(buffer->data + buffer->next, cadena, stringSize);
    buffer->next += stringSize;
}

void serialize_end_of_string(Buffer *buffer){
	serialize_ending_special_character("\0", buffer);
}

//Contatena un caracter especial de finalización de estructura
void serialize_ending_special_character(char *specialCharacter, Buffer *buffer){
	serialize_string(specialCharacter, buffer);
}

//Concatena integer en buffer utilizando PRIMITIVE_SEPARATOR como separador
void serialize_int(int integer, Buffer *buffer) {
    char *integerToString;
    integerToString = string_itoa(integer);

	serialize_string(integerToString, buffer);
}

void serialize_codeIndex(t_intructions* codeIndex, t_size instructionsCount, Buffer *buffer){
	if(instructionsCount > 0){
		int i;
		for(i = 0 ; i < instructionsCount ; i++){
			serialize_int(codeIndex[i].start, buffer);
			serialize_ending_special_character(PRIMITIVE_SEPARATOR, buffer);

			serialize_int(codeIndex[i].offset, buffer);
			serialize_ending_special_character(PRIMITIVE_SEPARATOR, buffer);

			serialize_ending_special_character(CODEINDEX_SEPARATOR, buffer);	//Fin de objeto
		}
		serialize_end_of_string(buffer->data);
	}
}

t_intructions* deserialize_codeIndex(char* serializedCodeIndex, t_size instructionsCount) {

	t_intructions *codeIndex = malloc((sizeof(t_intructions) * instructionsCount));

	char** deserializedList = string_split(serializedCodeIndex, CODEINDEX_SEPARATOR);
	char** deserializedInstruction;

	int i;
	for( i = 0 ; i < instructionsCount ; i++ ){
		deserializedInstruction = string_split(deserializedList[i], PRIMITIVE_SEPARATOR);

		codeIndex[i].start = atoi(deserializedInstruction[0]);
		codeIndex[i].offset = atoi(deserializedInstruction[1]);

		printf("--- Índice de código n° %d: %s \n", i, deserializedList[i]);
	}

	return codeIndex;
}

int convertToInt32(char* buffer){
	int32_t* number = malloc(sizeof(int32_t));
	memcpy(number, buffer, sizeof(int32_t));
	return *number;
}

char* serialize_pcb(PCB *pcb, Buffer *buffer){
	//TODO: Serializar

	//PID
	serialize_int(pcb->processId, buffer);
	serialize_ending_special_character(PCBSTRUCT_SEPARATOR, buffer);
	//programCounter
	serialize_int(pcb->programCounter, buffer);
	serialize_ending_special_character(PCBSTRUCT_SEPARATOR, buffer);
	//codePagesCount
	serialize_int(pcb->codePagesCount, buffer);
	serialize_ending_special_character(PCBSTRUCT_SEPARATOR, buffer);
	//instructionsCount
	serialize_int(pcb->instructionsCount, buffer);
	serialize_ending_special_character(PCBSTRUCT_SEPARATOR, buffer);
	//codeIndex
	serialize_codeIndex(pcb->codeIndex, pcb->instructionsCount, buffer);
	serialize_ending_special_character(PCBSTRUCT_SEPARATOR, buffer);
	//tagIndex
	serialize_string(pcb->tagIndex, buffer);
	serialize_ending_special_character(PCBSTRUCT_SEPARATOR, buffer);
	//stackIndex
	//stack

	return (char*)buffer->data;
}
