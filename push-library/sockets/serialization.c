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


struct Buffer *new_buffer() {
	struct Buffer *b = malloc(sizeof(Buffer));

	b->data = malloc(INITIAL_SIZE);
	b->size = INITIAL_SIZE;
	b->next = 0;

	return b;
}

void buffer_free(Buffer* buffer) {
	free(buffer->data);
	free(buffer);
}

//Iterator que además recibe un buffer como parámetro
void dictionary_serialization_iterator(t_dictionary *self, void(*closure)(char*,void*, Buffer*), Buffer* buffer) {
	int table_index;
	for (table_index = 0; table_index < self->table_max_size; table_index++) {
		t_hash_element *element = self->elements[table_index];

		while (element != NULL) {
			closure(element->key, element->data, buffer);
			element = element->next;

		}
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
	}
}

void serialize_dictionary_element(char* key, void* value, Buffer* buffer){
	serialize_string(key, buffer);
	serialize_ending_special_character(PRIMITIVE_SEPARATOR, buffer);

	serialize_string((char*)value, buffer);
	serialize_ending_special_character(PRIMITIVE_SEPARATOR, buffer);

	serialize_ending_special_character(DICTIONARY_SEPARATOR, buffer);
}

//Deserializa un diccionario seteando los value como cadenas
t_dictionary* deserialize_dictionary(char* serializedDictionary, int elementsCount){
	t_dictionary* resultDictionary = dictionary_create();

	char** deserializedList = string_split(serializedDictionary, DICTIONARY_SEPARATOR);
	char** deserializedElement;

	int i;
	for( i = 0 ; i < elementsCount ; i++ ){
		deserializedElement = string_split(deserializedList[i], PRIMITIVE_SEPARATOR);
		dictionary_put(resultDictionary, deserializedElement[0], deserializedElement[1]);
	}
	return resultDictionary;
}

//Concatena los elementos de un diccionario
void serialize_dictionary(t_dictionary* dictionary, Buffer* buffer){
	dictionary_serialization_iterator(dictionary, serialize_dictionary_element, buffer);
}

//Concatena una estructura de tipo variable
void serialize_variable(t_variable* variable, Buffer* buffer){
	serialize_string(variable->id, buffer);
	serialize_ending_special_character(VARIABLE_SEPARATOR, buffer);

	serialize_int(variable->pageNumber, buffer);
	serialize_ending_special_character(VARIABLE_SEPARATOR, buffer);

	serialize_int(variable->offset, buffer);
	serialize_ending_special_character(VARIABLE_SEPARATOR, buffer);

	serialize_int(variable->size, buffer);
	serialize_ending_special_character(VARIABLE_SEPARATOR, buffer);
}

t_variable* deserialize_variable(char* serializedVariable){
	t_variable* variable = malloc(sizeof(t_variable));
	char** deserializedVariable = string_split(serializedVariable, VARIABLE_SEPARATOR);

	variable->id = atoi(deserializedVariable[0]);
	variable->pageNumber = atoi(deserializedVariable[1]);
	variable->offset = atoi(deserializedVariable[2]);
	variable->size = atoi(deserializedVariable[3]);

	return variable;
}
//Serializa un elemento del Stack
void serialize_stackContent(t_stackContent* content, Buffer* buffer){
	//Serializar cantidad de elementos en el diccionario
	serialize_int(dictionary_size(content->arguments), buffer);
	serialize_ending_special_character(STACKCONTENT_SEPARATOR, buffer);
	//serializar diccionario => arguments
	serialize_dictionary(content->arguments, buffer);
	serialize_ending_special_character(STACKCONTENT_SEPARATOR, buffer);

	//Serializar cantidad de elementos en el diccionario
	serialize_int(dictionary_size(content->variables), buffer);
	serialize_ending_special_character(STACKCONTENT_SEPARATOR, buffer);
	//serializar diccionario => variables
	serialize_dictionary(content->variables, buffer);
	serialize_ending_special_character(STACKCONTENT_SEPARATOR, buffer);

	//serializar t_puntero => returnAdress
	serialize_int(content->returnAddress, buffer);
	serialize_ending_special_character(STACKCONTENT_SEPARATOR, buffer);

	//serializar t_variable => returnVariable
	serialize_variable(content->returnVariable, buffer);
	serialize_ending_special_character(STACKCONTENT_SEPARATOR, buffer);
}

void serialize_stackIndex(t_list* stack,int stackCount, Buffer* buffer){
	//Recorrer lista serializando de a uno
	int i;
	t_stackContent* stackContent = malloc(sizeof(t_stackContent));
	for (i = 0 ; i < stackCount ; i++){
		stackContent = list_get(stack, i);
		serialize_stackContent(stackContent, buffer);
		serialize_ending_special_character(STACK_SEPARATOR, buffer);
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
		//printf("--- Índice de código n° %d: %s \n", i, deserializedList[i]);
	}
	return codeIndex;
}

t_list* deserialize_stack(char* serializedStack, int stackCount){
	t_list* stack = list_create();

	if(stackCount == 0) return stack;

	t_stackContent* stackContent = malloc(sizeof(t_stackContent));
	char** deserializedList = string_split(serializedStack, STACK_SEPARATOR);
	char** deserializedElement;

	int auxCounter;
	int i;
	for( i = 0 ; i < stackCount ; i++ ){
		deserializedElement = string_split(deserializedList[i], STACKCONTENT_SEPARATOR);

		auxCounter = atoi(deserializedElement[0]);
		stackContent->arguments = deserialize_dictionary(deserializedElement[1], auxCounter);

		auxCounter = atoi(deserializedElement[2]);
		stackContent->variables = deserialize_dictionary(deserializedElement[3], auxCounter);

		stackContent->returnAddress = atoi(deserializedElement[4]);

		stackContent->returnVariable = deserialize_variable(deserializedElement[5]);

		list_add(stack, stackContent);
	}

	return stack;
}

int32_t convertToInt32(char* buffer){
	int32_t* number = malloc(sizeof(int32_t));
	memcpy(number, buffer, sizeof(int32_t));
	return *number;
}

char* serialize_pcb(PCB *pcb, Buffer *buffer){
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
	//memoryIndex
	serialize_int(pcb->memoryIndex, buffer);
	serialize_ending_special_character(PCBSTRUCT_SEPARATOR, buffer);

	//codeIndex
	serialize_codeIndex(pcb->codeIndex, pcb->instructionsCount, buffer);
	serialize_ending_special_character(PCBSTRUCT_SEPARATOR, buffer);
	//tagIndexSize
	serialize_int(pcb->tagIndexSize, buffer);
	serialize_ending_special_character(PCBSTRUCT_SEPARATOR, buffer);

	//tagIndex
	if(pcb->tagIndex != 0){
		serialize_string(pcb->tagIndex, buffer);
	}
	serialize_ending_special_character(PCBSTRUCT_SEPARATOR, buffer);
	//stackCount
	serialize_int(pcb->stackCount, buffer);
	serialize_ending_special_character(PCBSTRUCT_SEPARATOR, buffer);
	//stack
	serialize_stackIndex(pcb->stack, pcb->stackCount, buffer);
	serialize_ending_special_character(PCBSTRUCT_SEPARATOR, buffer);

	//guti
	serialize_int(pcb->guti, buffer);
	serialize_ending_special_character(PCBSTRUCT_SEPARATOR, buffer);

	return (char*)buffer->data;
}

PCB* deserialize_pcb(char* serializedPCB){
	PCB* pcb = new_pcb(0);
	char** serializedComponents = string_split(serializedPCB, PCBSTRUCT_SEPARATOR);

	pcb->processId = atoi(serializedComponents[0]);						//PID
	pcb->programCounter = atoi(serializedComponents[1]);				//programCounter
	pcb->codePagesCount = atoi(serializedComponents[2]);				//codePagesCount
	pcb->instructionsCount = atoi(serializedComponents[3]);				//instructionsCount
	pcb->memoryIndex = atoi(serializedComponents[4]);					//memoryIndex
	pcb->codeIndex = deserialize_codeIndex(serializedComponents[5], pcb->instructionsCount);	//codeIndex
	pcb->tagIndexSize = atoi(serializedComponents[6]);					//tagIndexSize
	pcb->tagIndex = serializedComponents[7];							//tagIndex
	pcb->stackCount = atoi(serializedComponents[8]);					//stackCount

	pcb->stack = deserialize_stack(serializedComponents[9], pcb->stackCount);
	pcb->guti = atoi(serializedComponents[10]);

	return pcb;
}

char* serialize_globalVar(t_globalVar* var, Buffer* buffer){
	serialize_string(var->varName, buffer);
	serialize_ending_special_character(PRIMITIVE_SEPARATOR, buffer);

	serialize_int(var->value, buffer);
	serialize_ending_special_character(PRIMITIVE_SEPARATOR, buffer);

	return buffer->data;
}

t_globalVar* deserialize_globalVar(char* serializedVar){
	t_globalVar* var = malloc(sizeof(t_globalVar));
	char** deserializedElement = string_split(serializedVar, PRIMITIVE_SEPARATOR);

	var->varName = deserializedElement[0];
	var->value = atoi(deserializedElement[1]);

	return var;
}
