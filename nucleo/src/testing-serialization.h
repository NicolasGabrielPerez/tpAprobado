#include <parser/metadata_program.h>
#include <parser/parser.h>
#include <sockets/serialization.h>
#include <sockets/pcb.h>

#ifndef TESTING_SERIALIZATION_H_
#define TESTING_SERIALIZATION_H_

void print_program_metadata(t_metadata_program *programMetadata);

void print_program_instruction_index(t_intructions *instructionIndex, t_size instructionsCount);

void test_serialization();

#endif /* TESTING_SERIALIZATION_H_ */
