#include <parser/metadata_program.h>
#include <parser/parser.h>
#include <sockets/serialization.h>
#include <sockets/pcb.h>
#include <stdio.h>
#include <stdlib.h>

t_list* General_Process_List;

int auxCounter;

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

void print_dictionary_element(char* key, void* value){
	printf("Entrada n°: %d\n", auxCounter);
	printf("-- Key: %s\n", key);
	printf("-- Value: %s\n", (char*)value);
	auxCounter ++;
}

void print_dictionary(t_dictionary* dictionary){
	auxCounter = 0;
	dictionary_iterator(dictionary, print_dictionary_element);
}

//TODO: Serializar estructura de stack


void initialize_pcb_test(PCB *pcb){
	pcb->processId = 493;
	pcb->programCounter = 0;
	pcb->codePagesCount = 25;
	pcb->stackIndex = 10;
}

t_dictionary* create_testing_dictionary(int dictionarySize){
	t_dictionary* testDictionary = dictionary_create();

	int i;

	char* key = string_new();
	char* value = string_new();
	char* aux = string_new();

	for (i = 0 ; i < dictionarySize ; i++){
		key = string_new();
		value = string_new();
		string_append(&key, "Key ");
		string_append(&key, string_itoa(i));
		string_append(&value, "Value ");
		string_append(&value, string_itoa(i));

		dictionary_put(testDictionary, key, value);
	}

	return testDictionary;
}

//Agrega elementos en la lista general de PCBs
void set_pcb_test_list(){
	PCB* pcb1 = malloc(sizeof(PCB));
	pcb1 = new_pcb();
	pcb1->tagIndex = "Hola guti\n";
	list_add(General_Process_List, pcb1);

	PCB* pcb2 = malloc(sizeof(PCB));
	pcb2 = new_pcb();
	pcb2->tagIndex = "Cómo andás?\n";
	list_add(General_Process_List, pcb2);

	PCB* pcb3 = malloc(sizeof(PCB));
	pcb3 = new_pcb();
	pcb3->tagIndex = "PCB 3\n";
	list_add(General_Process_List, pcb3);

	PCB* pcb4 = malloc(sizeof(PCB));
	pcb4 = new_pcb();
	pcb4->tagIndex = "PCB 4\n";
	list_add(General_Process_List, pcb4);

	list_remove(General_Process_List, 2);

	PCB* pcb5 = malloc(sizeof(PCB));
	pcb5 = new_pcb();
	pcb5->tagIndex = "PCB 5\n";
	list_add(General_Process_List, pcb5);
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

	printf("Posición de etiqueta principal: %d \n", instruccion);

	Buffer *dataBuffer = malloc(sizeof(Buffer));
	dataBuffer = new_buffer();

	//---------------------DICCIONARIOS

	t_dictionary* testDictionary = create_testing_dictionary(5);

	serialize_dictionary(testDictionary, dataBuffer);
	free(testDictionary);
	printf("Diccionario serializado: %s \n", (char*)dataBuffer->data);

	t_dictionary* auxDictionary = malloc(sizeof(t_dictionary));
	auxDictionary = deserialize_dictionary(dataBuffer->data, 5);

	print_dictionary(auxDictionary);


	//t_dictionary* testDictionary = dictionary_create();
	//dictionary_put(testDictionary, "key1\0", "value1\0");

	//---------------------DICCIONARIOS



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
	*/

	free(programMetadata);

	//t_intructions* programInstructionIndex = codeIndex;

	//print_program_instruction_index(programInstructionIndex, programMetadata->instrucciones_size);


}

void test_planification(){
	if(General_Process_List == NULL){
		General_Process_List = list_create();
	}
	set_pcb_test_list();

	PCB* pcb = malloc(sizeof(PCB));
	int i;
	for(i = 0 ; i < list_size(General_Process_List) ; i++){
		pcb = get_pcb_by_ID(General_Process_List, i);
		if(pcb != NULL){
			printf("Process ID: %d\n", pcb->processId);
			printf("--%s\n", pcb->tagIndex);
		}
	}
}
