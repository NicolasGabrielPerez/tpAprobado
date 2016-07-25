#ifndef SERIALIZATION_H_
#define SERIALIZATION_H_

#include <stdlib.h>
#include <string.h>
#include <parser/metadata_program.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include "pcb.h"

#define INITIAL_SIZE 32
#define PCB_ELEMENTS_COUNT 9
#define PRIMITIVE_SEPARATOR "!"
#define CODEINDEX_SEPARATOR "#"
#define PCBSTRUCT_SEPARATOR "$"
#define DICTIONARY_SEPARATOR "|"
#define VARIABLE_SEPARATOR "¡"
#define STACKCONTENT_SEPARATOR "°"
#define STACK_SEPARATOR "&"

#define EMPTYVALUE_IDENTIFIER "_"

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

	typedef struct globalVar{
		char* varName;
		int32_t value;
	} t_globalVar;

	char* serializarResponse(response* response, int* responseSize);

	struct Buffer *new_buffer();
	void buffer_free(Buffer* buffer);

	void dictionary_serialization_iterator(t_dictionary *self, void(*closure)(char*,void*, Buffer*), Buffer* buffer);

	//Reserva espacio dinámicamente y aumenta el tamaño del buffer bajo demanda
	void reserve_space(Buffer *buffer, size_t bytesNeeded);

	//Concatena string en buffer utilizando PRIMITIVE_SEPARATOR como separador
	void serialize_string(char* string, Buffer *buffer);

	void serialize_end_of_string(Buffer *buffer);

	//Contatena un caracter especial de finalización de estructura
	void serialize_ending_special_character(char *specialCharacter, Buffer *buffer);

	//Concatena integer en buffer utilizando PRIMITIVE_SEPARATOR como separador
	void serialize_int(int integer, Buffer *buffer);

	void serialize_codeIndex(t_intructions* codeIndex, t_size instructionsCount, Buffer *buffer);

	void serialize_stackIndex(t_list* stack,int stackCount, Buffer* buffer);

	void serialize_dictionary_element(char* key, void* value, Buffer* buffer);

	void serialize_dictionary(t_dictionary* dictionary, Buffer* buffer);

	void serialize_variable(t_variable* variable, Buffer* buffer);

	void serialize_stackContent(t_stackContent* content, Buffer* buffer);

	t_intructions* deserialize_codeIndex(char* serializedCodeIndex, t_size instructionsCount);

	t_dictionary* deserialize_dictionary(char* serializedDictionary, int elementsCount);

	int32_t convertToInt32(char* buffer);

	char* serialize_pcb(PCB *pcb, Buffer *buffer);
	PCB* deserialize_pcb(char* serializedPCB);

	char* serialize_globalVar(t_globalVar* var, Buffer* buffer);
	t_globalVar* deserialize_globalVar(char* serializedVar);

#endif
