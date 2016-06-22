#include <parser/metadata_program.h>
#include <parser/parser.h>
#include <sockets/serialization.h>
#include <sockets/pcb.h>
#include <stdio.h>
#include <stdlib.h>

#define PRIMITIVE_SEPARATOR "$!"
#define CODEINDEX_SEPARATOR "$#"

void print_program_metadata(t_metadata_program *programMetadata) {
	puts("++++++++++++Program Metadada++++++++++++ \n");
	printf("t_size: %d \n", programMetadata->instrucciones_size);
	printf("Instrucción inicio: %d \n", programMetadata->instruccion_inicio);

	int j = 0;
	//int k = 0;
	printf("Cantidad de etiquetas: %d \n",
			programMetadata->cantidad_de_etiquetas);
	printf("Cantidad de funciones: %d \n",
			programMetadata->cantidad_de_funciones);

	puts("------------------Etiquetas------------------ \n");
	//for(k = 0 ; k < programMetadata->cantidad_de_funciones ; k ++){
	//	printf("La etiqueta %d es: %s \n", k, programMetadata->etiquetas[k]);
	//}
	printf("Etiquetas: %s \n", programMetadata->etiquetas);
	printf("Tamaño de etiquetas: %d \n", programMetadata->etiquetas_size);

	//t_puntero_instruccion tpi =  metadata_buscar_etiqueta("triple", programMetadata->etiquetas, programMetadata->etiquetas_size);
	//printf("Posición etiqueta: %d \n", tpi);

	puts("------------------Instrucciones------------------");
	printf("Cantidad de instrucciones: %d \n \n",
			programMetadata->instrucciones_size);
	for (j = 0; j < programMetadata->instrucciones_size; j++) {
		printf("**Comienzo instrucción %d: %d \n", j,
				programMetadata->instrucciones_serializado[j].start);
		printf("****Offset %d: %d \n", j,
				programMetadata->instrucciones_serializado[j].offset);
	}

	//char **prueba = string_split(programMetadata->etiquetas, "\0");
	//printf(">>>>>>>>>>>>>>>>>>PRobando %s, %d, %d, %d \n", prueba[0], prueba[1],prueba[2],prueba[3]);

}

void print_program_instruction_index(t_intructions *instructionIndex, t_size instructionsCount){
	printf("---------- Índice de instrucciones ---------- \n");

	int i;
	for (i = 0 ; i < instructionsCount ; i++){
		printf("Índice instrucción %d: \n", i);
		printf("---- Start: %d \n", instructionIndex[i].start);
		printf("---- Offset: %d \n", instructionIndex[i].offset);
	}

	printf("---------- Fín de índice ---------- \n");
}

//TODO: Serializar índice de etiquetas

//TODO: Serializar estructura de stack

//Serializa la estructura de código de programa entera
void serialize_pcb(PCB *pcb, Buffer *output) {
    serialize_int(pcb->processId, output);
    serialize_int(pcb->programCounter, output);
    serialize_int(pcb->codePagesCount, output);
    serialize_int(pcb->stackIndex, output);
}

void deserialize_string(Buffer *buffer){
	char** vector = string_split((char*)buffer->data, PRIMITIVE_SEPARATOR);

	int i;

	for(i = 0 ; i < 2 ; i++){
		printf("Vector en %d contiene: %s \n", i, vector[i]);
	}
}

void initialize_pcb_test(PCB *pcb){
	pcb->processId = 493;
	pcb->programCounter = 0;
	pcb->codePagesCount = 25;
	pcb->stackIndex = 10;
}

void test_serialization(){

	char *programa1 = "function triple\n"
			"variables f\n"
			"f = $0 + $0 + $0\n"
			"return f\n"
			"end\n"

			"begin \n"
			"variables a,g \n"
			"a = 1\n"
			"g <- doble a\n"
			"print g\n"
			"end\n"

			"function doble\n"
			"variables f\n"
			"f = $0 + $0\n"
			"return f\n"
			"end";

	t_metadata_program *programMetadata = malloc(sizeof(t_metadata_program));
	programMetadata = metadata_desde_literal(programa1);

	//Cantidad de páginas de memoria
	int programPages = getProgramPagesCount(programa1);

	//Posición de etiqueta de programa principal
	t_puntero_instruccion instruccion = metadata_buscar_etiqueta("begin", programMetadata->etiquetas, programMetadata->etiquetas_size);

	printf("Posición de etiqueta principal: %d", instruccion);

	Buffer *dataBuffer = malloc(sizeof(Buffer));
	dataBuffer = new_buffer();

	serialize_codeIndex(programMetadata->instrucciones_serializado, programMetadata->instrucciones_size, dataBuffer);

	t_intructions* programInstructionIndex = deserialize_codeIndex((char*)dataBuffer->data, programMetadata->instrucciones_size);

	/*
	char** deserializedList = string_split(serializedCodeIndex, CODEINDEX_SEPARATOR);
	char** deserializedInstruction;
	puts("Serializado spliteado \n");
	int i;
	for( i = 0 ; i < programMetadata->instrucciones_size ; i++ ){
		deserializedInstruction = string_split(deserializedList[i], PRIMITIVE_SEPARATOR);
		codeIndex[i].start = atoi(deserializedInstruction[0]);
		codeIndex[i].offset = atoi(deserializedInstruction[1]);
		printf("--- Índice de código n° %d: %s \n", i, deserializedList[i]);
	}

	free(programMetadata);
*/
	//t_intructions* programInstructionIndex = codeIndex;

	print_program_instruction_index(programInstructionIndex, programMetadata->instrucciones_size);


}
