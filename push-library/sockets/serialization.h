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

	void serialize_end_of_string(Buffer *buffer);

	//Contatena un caracter especial de finalización de estructura
	void serialize_ending_special_character(char *specialCharacter, Buffer *buffer);

	//Concatena integer en buffer utilizando PRIMITIVE_SEPARATOR como separador
	void serialize_int(int integer, Buffer *buffer);

	void serialize_codeIndex(t_intructions* codeIndex, t_size instructionsCount, Buffer *buffer);

	t_intructions* deserialize_codeIndex(char* serializedCodeIndex, t_size instructionsCount);

	int convertToInt32(char* buffer);

#endif
