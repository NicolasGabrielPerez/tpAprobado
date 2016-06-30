#include <parser/metadata_program.h>
#include <parser/parser.h>
#include <sockets/serialization.h>
#include <sockets/pcb.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>

#ifndef TESTING_SERIALIZATION_H_
#define TESTING_SERIALIZATION_H_

extern t_list* General_Process_List;

void print_program_metadata(t_metadata_program *programMetadata);

void print_program_instruction_index(t_intructions *instructionIndex, t_size instructionsCount);

void print_dictionary_element(char* key, void* value);

void print_dictionary(t_dictionary* dictionary);

t_dictionary* create_testing_dictionary(int dictionarySize);

void test_serialization();

#endif /* TESTING_SERIALIZATION_H_ */
