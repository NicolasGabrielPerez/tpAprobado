#include "nucleo-comando.h"

#define OK       0
#define NO_INPUT 1
#define TOO_LONG 2


	t_queue*  READY_Process_Queue;

	t_queue* BLOCKED_Process_Queue;

	t_list* General_Process_List;
	t_list* RUNNING_Process_List;

	t_list*  IO_Device_List;
	t_list* CPU_control_list;
	t_list*  semaforo_control_list;

static int getLine (char *prmpt, char *buff, size_t sz) {
	int ch, extra;

	// Get line with buffer overrun protection.
	if (prmpt != NULL) {
		printf ("%s", prmpt);
		fflush (stdout);
	}
	if (fgets (buff, sz, stdin) == NULL)
		return NO_INPUT;

	// If it was too long, there'll be no newline. In that case, we flush
	// to end of line so that excess doesn't affect the next call.
	if (buff[strlen(buff)-1] != '\n') {
		extra = 0;
		while (((ch = getchar()) != '\n') && (ch != EOF))
			extra = 1;
		return (extra == 1) ? TOO_LONG : OK;
	}

	// Otherwise remove newline and give string back to caller.
	buff[strlen(buff)-1] = '\0';
	return OK;
}

void parsearDumpVar(char* command){
	int offset = sizeof("dump variables") - 1;
	//char* sPid = string_substring_from(command, offset);

	//int pid = strtol(sPid, NULL, 10);

	//if(pid <= 0){
		printVar();
		return;
	//}

	//dumpTable(pid);//esto
}

void* imprimirVariable(char* key, void* value){

	printf("Nombre: %s | valor=%d \n",key, (int)value);
	return 0;
}

void printVar(){

	printf("----------------- Comienzo de variables -----------------\n");
	dictionary_iterator(vars_control_dictionary, imprimirVariable);

	printf("----------------- Fin de variables -----------------\n");
}

void parsearDumpCola(char* command){
	int offset = sizeof("dump colas") - 1;
	//char* sPid = string_substring_from(command, offset);

	//int pid = strtol(sPid, NULL, 10);

	//if(pid <= 0){
	printColas();
	return;
	//}

	//dumpTable(pid);//
}

void printColas(){
	printf("----------------- Comienzo colas -----------------\n");

	t_queue* redy = READY_Process_Queue;
	t_list* listaReady =redy->elements;
	t_queue* block= BLOCKED_Process_Queue;
	t_list* listaBlock = block->elements;
	t_list* general =General_Process_List;
	t_list* run = RUNNING_Process_List;

	t_list* ioList = IO_Device_List;
	t_list* cpuList = CPU_control_list;
	t_list* semaforoList = semaforo_control_list;


	int i;
	PCB* entry;
	PCB* entry2;
	PCB* unPCB;
	PCB* unPCB2;
	t_IO_Device* auxDevice;

	for(i = 0; i< list_size(listaReady); i++){
		entry = (PCB*)list_get(listaReady, i);
		printf("Cola De READY | PID = %d\n",
				entry->processId);
		printf("------- Fin entrada de cola -------\n");
	}
	for(i = 0; i< list_size(listaBlock); i++){
		entry2 = (PCB*)list_get(listaBlock, i);
		printf("Cola De BLOCKED | PID=%d\n",
				entry2->processId);
		printf("------- Fin entrada de cola -------\n");
	}
	for(i = 0; i< list_size(general); i++){
		unPCB = list_get(general, i);
		printf("Cola De General_Process | PID=%d\n",
				unPCB->processId);
		printf("------- Fin entrada de cola -------\n");
	}
	for(i = 0; i< list_size(run); i++){
		unPCB2 = list_get(run, i);
		printf("Cola De RUNNING | PID=%d\n",
				unPCB2->processId);
		printf("------- Fin entrada de cola -------\n");
	}

	printf("Lista De IO_Device\n");
	for(i = 0; i< list_size(ioList); i++){
		auxDevice = list_get(ioList, i);
		printf("IO_Device => ioId = %s | sleepTime = %d | Blocked process count = %d\n",
				auxDevice->ioId, auxDevice->sleepTime, queue_size(auxDevice->BlockedProcessesQueue));
		printf("------- Fin entrada de cola -------\n");
	}
	printf("------- Fin lista dispositivos -------\n");

	printf("Lista De CPUs conectadas\n");
	for(i = 0; i< list_size(cpuList); i++){
		t_CPU* unaCPU = (t_CPU*)list_get(cpuList, i);
		printf("CPU => Socket = %d | PID = %d\n",
				unaCPU->cpuSocket, unaCPU->PID);
		printf("------- Fin entrada de cola -------\n");
	}
	printf("------- Fin lista de CPUs -------\n");

	printf("Lista De Semáforos\n");
	t_queue* auxQueue;
	for(i = 0; i < list_size(semaforoList); i++){
		t_semaforo* unSemaforo = list_get(semaforoList, i);
		printf("Semáforo => sem_id = %s | Value = %d | Blocked process count = %d\n",
				unSemaforo->sem_id, unSemaforo->sem_value, queue_size(unSemaforo->blocked_process_queue));
		printf("------- Fin entrada de lista -------\n");
	}
	printf("------- Fin lista de Semáforos -------\n");

	printf("----------------- Fin de colas -----------------\n");
}

//variables comp y cola de proseso
void* consolaDeComandos(){
	printf("--------------- Bienvenido a la consola del NUCLEO ---------------\n");
	printf("Estos son los comandos disponibles:\n");
	printf("dump variables *info: ver las variables compartidas\n");
	printf("dump colas *info: ver colas disponibles\n");
	printf("----------------------------------------------------------------------------\n");

	int rc;


	while(1){
		char* buff = malloc(30);
		rc = getLine ("Esperando comando> ", buff, 30);
		if (rc == NO_INPUT) {
			// Extra NL since my system doesn't output that on EOF.
			printf ("\nSin input de comando\n");
			free(buff);
			continue;
		}

		if (rc == TOO_LONG) {
			printf ("Comando demasiado largo [%s]\n", buff);
			free(buff);
			continue;
		}

		ejecutarComando(buff);
		//free(buff);
	}
}


void ejecutarComando(char* command){
	string_trim_left(&command);

	if(string_starts_with(command, "dump variables")){
		parsearDumpVar(command);
		return;
	}
	if(string_starts_with(command, "dump colas")){
		parsearDumpCola(command);
		return;
	}

	printf("Comando invalido\n");
}
void initNUCLEOConsole(pthread_attr_t nucleo_attr){
	pthread_t newThread;
	pthread_create(&newThread, &nucleo_attr, &consolaDeComandos, NULL);
}
