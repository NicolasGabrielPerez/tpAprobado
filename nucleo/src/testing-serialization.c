#include <parser/metadata_program.h>
#include <parser/parser.h>
#include <sockets/serialization.h>
#include <sockets/pcb.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "cpu-interfaz.h"
#include "planificador.h"

pthread_attr_t nucleo_attr;
t_list* General_Process_List;

int auxCounter;
char* programa1 = "function triple\n"
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
		"end"

		"function guti\n"
		"variables f\n"
		"f = $0 + $0\n"
		"return f\n"
		"end"

		"function unafuncionmaslargaquelamierda\n"
		"variables f\n"
		"f = $0 + $0\n"
		"return f\n"
		"end";

char* programa2 = "begin \n"
		"variables a,g \n"
		"a = 1\n"
		"g <- a\n"
		"print g\n"
		"end\n";

char* programa3 = "begin\n"
		"variables f,  A,  g\n"
		"A = 	0\n"
		"!compartida = 1+A\n"
		"print !compartida\n"
		"jnz !compartida Siguiente\n"
		":Proximo\n"

		"f = 8\n"
		"g <- doble !compartida\n"
		"io LPT1 20\n"

		"textPrint    Hola Mundo!\n"

		"g = 1 + g\n"
		"print 		g\n"
		"textPrint Bye\n"
		"end\n"

		"#Devolver el doble del\n"
		"#primer parametro\n"
		"function doble\n"
		"variables f\n"
		"f = $0 + $0\n"
		"return f\n"
		"end\n"

		":Siguiente\n"
		"print A+1\n"
		"goto Proximo\n";

char* programa4 = "begin"
		"variables f,  A,  g"
		"A = 	0"
		"!compartida = 1+A"
		"print !compartida"
		"jnz !compartida Siguiente"
		":Proximo"

		"f = 8"
		"g <- doble !compartida"
		"io LPT1 20"

		"textPrint    Hola Mundo!"

		"g = 1 + g"
		"print 		g"
		"textPrint Bye"
		"end"

		"#Devolver el doble del"
		"#primer parametro"
		"function doble"
		"variables f"
		"f = $0 + $0"
		"return f"
		"end"

		":Siguiente"
		"print A+1"
		"goto Proximo";

void testDictionaryElement(char* key, void* value){
	printf("Variable: %s    -    Valor: %s\n", key, string_from_format("%s", value));
}

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

void initialize_pcb_test(PCB *pcb){
	pcb->processId = 493;
	pcb->programCounter = 0;
	pcb->codePagesCount = 25;
	//pcb->stackIndex = 10;
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
	pcb1 = new_pcb(0);
	pcb1->tagIndex = "Hola guti\n";
	list_add(General_Process_List, pcb1);

	PCB* pcb2 = malloc(sizeof(PCB));
	pcb2 = new_pcb(1);
	pcb2->tagIndex = "Cómo andás?\n";
	list_add(General_Process_List, pcb2);

	PCB* pcb3 = malloc(sizeof(PCB));
	pcb3 = new_pcb(2);
	pcb3->tagIndex = "PCB 3\n";
	list_add(General_Process_List, pcb3);

	PCB* pcb4 = malloc(sizeof(PCB));
	pcb4 = new_pcb(3);
	pcb4->tagIndex = "PCB 4\n";
	list_add(General_Process_List, pcb4);

	//list_remove(General_Process_List, 2);

	PCB* pcb5 = malloc(sizeof(PCB));
	pcb5 = new_pcb(4);
	pcb5->tagIndex = "PCB 5\n";
	list_add(General_Process_List, pcb5);
}

void test_serialization(){

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
	PCB* newPcb = malloc(sizeof(PCB));

	newPcb = new_pcb(3);
	create_program_PCB(newPcb, programa1, 30);
	print_program_instruction_index(newPcb->codeIndex, newPcb->instructionsCount);

	nucleo_updatePCB(newPcb);
	pcb = get_pcb_by_ID(General_Process_List, 3);
	print_program_instruction_index(pcb->codeIndex, pcb->instructionsCount);

	/*
	int i;
	for(i = 0 ; i < list_size(General_Process_List) ; i++){
		pcb = get_pcb_by_ID(General_Process_List, i);
		if(pcb != NULL){
			printf("Process ID: %d\n", pcb->processId);
			printf("--%s\n", pcb->tagIndex);
		}
	}*/
}

void test_cpu_communication(){
	//Conectarme con una instancia de CPU
	//Hilo de select
	init_threads_config(nucleo_attr);
	init_cpu_communication_thread(nucleo_attr);

	init_planification_thread(nucleo_attr);

	//Genero una estructura de programa (PCB) nuevo
	PCB* pcb = new_pcb(0);
	create_program_PCB(pcb, programa1, 100);

	add_pcb_to_general_list(pcb);
	set_pcb_READY(pcb);
	//Creo hilos

	//Envío de mensajes
}

void test_PCB_serialization(){
	char* programa = programa3;

	PCB* pcb1 = new_pcb(0);
	create_program_PCB(pcb1, programa, 45);

	Buffer* buffer = new_buffer();

	int j;


	puts(" - PCB 1:\n");
	printf("TagIndexSize 1: %d\n", pcb1->tagIndexSize);
	for(j = 0 ; j < pcb1->tagIndexSize ; j++){
		printf("Elemento %d = %c \n", j, pcb1->tagIndex[j]);
	}

	//Agregar elementos al stack
	t_stackContent* stackContent = init_stackContent();
	stackContent->returnAddress = 10;
	stackContent->returnVariable = 20;
	dictionary_put(stackContent->variables, "var1", "9");
	dictionary_put(stackContent->variables, "var2", "30");
	dictionary_put(stackContent->variables, "var3", "100");
	dictionary_put(stackContent->variables, "a", "1234567890");

	list_add(pcb1->stack, stackContent);
	pcb1->stackCount++;

	t_stackContent* stackContent2 = init_stackContent();
	stackContent2->returnAddress = 10;
	stackContent2->returnVariable = 20;
	dictionary_put(stackContent2->variables, "var4", "9");
	dictionary_put(stackContent2->variables, "var5", "30");
	dictionary_put(stackContent2->variables, "var6", "100");
	dictionary_put(stackContent2->variables, "b", "2345678901");

	list_add(pcb1->stack, stackContent2);
	pcb1->stackCount++;

	char* serializedPCB = serialize_pcb(pcb1, buffer);
	printf("cadena serializada: %s \n", serializedPCB);
	//Revertir formateo de tag Index
	//	puts("Desconvirtiendo\n");
	//	for(j = 0 ; j < pcb1->tagIndexSize ; j++){
	//		if(pcb1->tagIndex[j] == '.'){
	//			pcb1->tagIndex[j] = '\0';
	//		}
	//	}

	PCB* pcb2 = deserialize_pcb(serializedPCB);
	puts(" - PCB 2:\n");
	printf("TagIndexSize 2: %d\n", pcb2->tagIndexSize);
	puts("Desconvirtiendo\n");
	for(j = 0 ; j < pcb2->tagIndexSize ; j++){
		if(pcb2->tagIndex[j] == '.'){
			pcb2->tagIndex[j] = '\0';
		}
		printf("PCB 2 - Elemento %d = %c \n", j, pcb2->tagIndex[j]);
	}
	printf("TagIndex 2: %s\n", pcb2->tagIndex);

	char* etiqueta1 = "Siguiente";
	char* etiqueta2 = "Proximo";
	char* etiqueta3 = "doble";


	puts("***************************-------------------TEST - Tag index\n");
	printf("PCB 1 - TagIndex: %s\n", pcb1->tagIndex);
	t_puntero_instruccion puntero = metadata_buscar_etiqueta(etiqueta1, pcb1->tagIndex, pcb1->tagIndexSize);
	printf("Posición de puntero %s: %d\n", etiqueta1, puntero);

	puntero = metadata_buscar_etiqueta(etiqueta2, pcb1->tagIndex, pcb1->tagIndexSize);
	printf("Posición de puntero %s: %d\n", etiqueta2, puntero);

	puntero = metadata_buscar_etiqueta(etiqueta3, pcb1->tagIndex, pcb1->tagIndexSize);
	printf("Posición de puntero %s: %d\n", etiqueta3, puntero);

	puntero = metadata_buscar_etiqueta("unafuncionmaslargaquelamierda", pcb1->tagIndex, pcb1->tagIndexSize);
	printf("Posición de puntero %s: %d\n", "unafuncionmaslargaquelamierda", puntero);

	puts("PCB Serializado\n");
	printf("PCB 2 - TagIndex: %s\n", pcb2->tagIndex);
	puntero = metadata_buscar_etiqueta(etiqueta1, pcb2->tagIndex, pcb2->tagIndexSize);
	printf("Posición de puntero 2 %s: %d\n", etiqueta1, puntero);

	puntero = metadata_buscar_etiqueta(etiqueta2, pcb2->tagIndex, pcb2->tagIndexSize);
	printf("Posición de puntero 2 %s: %d\n", etiqueta2, puntero);

	puntero = metadata_buscar_etiqueta(etiqueta3, pcb2->tagIndex, pcb2->tagIndexSize);
	printf("Posición de puntero 2 %s: %d\n", etiqueta3, puntero);

	puntero = metadata_buscar_etiqueta("unafuncionmaslargaquelamierda", pcb2->tagIndex, pcb2->tagIndexSize);
	printf("Posición de puntero 2 %s: %d\n", "unafuncionmaslargaquelamierda", puntero);

	puts("Inicio de comparación\n");
	if(pcb1->processId != pcb2->processId){
		printf("ERROR: PID 1 = %d | PID 2 = %d\n", pcb1->processId, pcb2->processId);
	}

	if(pcb1->codePagesCount != pcb2->codePagesCount){
		printf("ERROR: codePagesCount 1 = %d | codePagesCount 2 = %d\n", pcb1->codePagesCount, pcb2->codePagesCount);
	}

	if(pcb1->stackInitPosition != pcb2->stackInitPosition){
		printf("ERROR: guti 1 = %d | guti 2 = %d\n", pcb1->stackInitPosition, pcb2->stackInitPosition);
	}

	if(pcb1->instructionsCount != pcb2->instructionsCount){
		printf("ERROR: instructionsCount 1 = %d | instructionsCount 2 = %d\n", pcb1->instructionsCount, pcb2->instructionsCount);
	}

	if(pcb1->memoryIndex != pcb2->memoryIndex){
		printf("ERROR: memoryIndex 1 = %d | memoryIndex 2 = %d\n", pcb1->memoryIndex, pcb2->memoryIndex);
	}

	if(pcb1->tagIndexSize != pcb2->tagIndexSize){
		printf("ERROR: tagIndexSize 1 = %d | tagIndexSize 2 = %d\n", pcb1->tagIndexSize, pcb2->tagIndexSize);
	}

	if(pcb1->stackCount != pcb2->stackCount){
		printf("ERROR: stackCount 1 = %d | stackCount 2 = %d\n", pcb1->stackCount, pcb2->stackCount);
	}
	if(pcb1->programCounter != pcb2->programCounter){
		printf("ERROR: programCounter 1 = %d | programCounter 2 = %d\n", pcb1->programCounter, pcb2->programCounter);
	}
	if(list_size(pcb1->stack) != list_size(pcb2->stack)){
		printf("ERROR: Stack size 1 = %d | Stack size 2 = %d\n", list_size(pcb1->stack), list_size(pcb2->stack));
	}

	int i;
	t_stackContent* testStackContent1;
	t_stackContent* testStackContent2;
	for( i = 0 ; i < pcb2->stackCount ; i++){
		testStackContent1 = list_get(pcb1->stack, i);
		testStackContent2 = list_get(pcb2->stack, i);

		if(testStackContent1->returnAddress != testStackContent2->returnAddress){
			printf("ERROR: StackContent[%d] returnAddress 1 = %d | returnAddress 2 = %d\n", i, testStackContent1->returnAddress, testStackContent2->returnAddress);
		}

		if(testStackContent1->returnVariable != testStackContent2->returnVariable){
			printf("ERROR: StackContent[%d] returnVariable 1 = %d | returnVariable 2 = %d\n", i, testStackContent1->returnVariable, testStackContent2->returnVariable);
		}

		if(dictionary_size(testStackContent1->variables) != dictionary_size(testStackContent2->variables)){
			printf("ERROR: StackContent[%d] variables size 1 = %d | variables size 2 = %d\n", i, dictionary_size(testStackContent1->variables), dictionary_size(testStackContent2->variables));
		}

		puts("Diccionario stack - PCB1");
		dictionary_iterator(testStackContent1->variables, testDictionaryElement);

		puts("Diccionario stack - PCB2");
		dictionary_iterator(testStackContent2->variables, testDictionaryElement);
	}

	for( i = 0 ; i < pcb2->instructionsCount ; i++ ){
		if(pcb1->codeIndex[i].start != pcb2->codeIndex[i].start){
			printf("ERROR: codeIndex1, inst %d start = %d | codeIndex2, inst %d = %d\n", i, pcb1->codeIndex[i].start, i, pcb2->codeIndex[i].start);
		}

		if(pcb1->codeIndex[i].offset != pcb2->codeIndex[i].offset){
			printf("ERROR: codeIndex1, inst %d offset = %d | codeIndex2, inst %d = %d\n", i, pcb1->codeIndex[i].offset, i, pcb2->codeIndex[i].offset);
		}
	}

	//if(strcmp(pcb1->tagIndex,pcb2->tagIndex)){
	//	printf("ERROR: tagIndex 1 = %s | tagIndex 2 = %s", pcb1->tagIndex, pcb2->tagIndex);
	//}

	puts("Fin de comparación\n");

}

void test_list_and_queues(){
	PCB* pcb0 = new_pcb(0);
	create_program_PCB(pcb0, programa1, 40);
	list_add(General_Process_List, pcb0);
	list_add(RUNNING_Process_List, pcb0);
	queue_push(READY_Process_Queue, pcb0);

	PCB* pcb1 = new_pcb(1);
	create_program_PCB(pcb1, programa1, 70);
	list_add(General_Process_List, pcb1);
	list_add(RUNNING_Process_List, pcb1);
	queue_push(READY_Process_Queue, pcb1);

	PCB* pcb2 = new_pcb(2);
	create_program_PCB(pcb2, programa1, 100);
	list_add(General_Process_List, pcb2);
	list_add(RUNNING_Process_List, pcb2);
	queue_push(READY_Process_Queue, pcb1);

	PCB* pcbAux3 = remove_pcb_by_ID(General_Process_List, 0);
	free_pcb(pcbAux3);
	PCB* pcbAux0 = get_pcb_by_ID(RUNNING_Process_List, 0);
	pcbAux3 = get_pcb_by_ID(RUNNING_Process_List, 0);

	PCB* pcbAux1 = queue_pop(READY_Process_Queue);
	printf("Cola de READY POP - ProcessId: %d\n", pcbAux1->processId);

	PCB* pcbAux4 = get_pcb_by_ID(General_Process_List, 0);
	if(pcbAux4 != NULL){
		printf("Proceso encontrado en lista\n");
	}
	else{
		printf("Proceso no encontrado en la lista\n");
	}

	pcb1->tagIndex = "Tag Index modificado desde pcb1";
	pcbAux4 = get_pcb_by_ID(RUNNING_Process_List, 1);
	printf("TagIndex modificado - Recibido desde RUNNING: %s\n", pcbAux4->tagIndex);

	pcbAux4 = queue_pop(READY_Process_Queue);
	printf("Cola de READY POP - ProcessId: %d\n", pcbAux4->processId);
	printf("TagIndex modificado - Recibido desde Cola de READY: %s\n", pcbAux4->tagIndex);


}
