#include "nucleo-comando.h"



#define OK       0
#define NO_INPUT 1
#define TOO_LONG 2


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


void ejecutarComando(char* command){
	string_trim_left(&command);

	if(string_starts_with(command, "dump vaiables")){
		parsearVar(command);
		return;
	}
	if(string_starts_with(command, "dump colas")){
		parsearDumpColas(command);
		return;
	}

	printf("Comando invalido\n");
}


void parsearDumpVar(char* command){
	int offset = sizeof("dump vaiables") - 1;
	char* sPid = string_substring_from(command, offset);

	int pid = strtol(sPid, NULL, 10);

	if(pid <= 0){
		printVar();
		return;
	}

	dumpTable(pid);//esto
}

void* imprimirVariable(char* key, void* value){

	printf("Nombre: %s | valor=%d \n",
			key, (int32_t)value);
return 0;
}

void printVar(){

	t_dictionary* vars_control_dictionary;

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
	t_list* listaRedy =redy->elements;
	t_queue* block= BLOCKED_Process_Queue;
	t_list* listaBlock = block->elements;
	t_list* general =General_Process_List;
	t_list* run = RUNNING_Process_List;

	t_list* io = IO_Device_List;
	t_list* cpu = CPU_control_list;
	t_list* semaforo = semaforo_control_list;


	int i;
		for(i = 0; i< list_size(listaRedy); i++){
		PCB entry = list_get(listaRedy, i);
		printf("Cola De READY | PID=%d\n",
				 entry.processId);
		printf("------- Fin entrada de cola -------\n");
	}
		for(i = 0; i< list_size(listaBlock); i++){
		PCB entry = list_get(listaBlock, i);
		printf("Cola De BLOCKED | PID=%d\n",
				 entry.processId);
		printf("------- Fin entrada de cola -------\n");
	}
		for(i = 0; i< list_size(general); i++){
			PCB* unPCB = list_get(general, i);
		printf("Cola De General_Process | PID=%d\n",
				unPCB->processId);
		printf("------- Fin entrada de cola -------\n");
	}
		for(i = 0; i< list_size(run); i++){
			PCB* unPCB = list_get(run, i);
			printf("Cola De RUNNING | PID=%d\n",
				 unPCB->processId);
		printf("------- Fin entrada de cola -------\n");
	}
		for(i = 0; i< list_size(io); i++){
			PCB* unPCB = list_get(io, i);
		printf("Cola De IO_Device | cpuID=%d\n",
				unPCB->processId);
		printf("------- Fin entrada de cola -------\n");
	}
		for(i = 0; i< list_size(cpu); i++){
			t_CPU* unPCB = list_get(cpu, i);
		printf("Cola De CPUs conectadas | PID=%d\n",
				unPCB->cpuSocket);
		printf("------- Fin entrada de cola -------\n");
	}
		for(i = 0; i< list_size(semaforo); i++){
			PCB* unPCB = list_get(semaforo, i);
		printf("Cola De semadoros | PID=%d\n",
				unPCB->processId);
		printf("------- Fin entrada de cola -------\n");
	}


	printf("----------------- Fin de colas -----------------\n");
}
//variables comp y cola de proseso
void* consolaDeComandos(){
	printf("--------------- Bienvenido a la consola del NUCLEO ---------------\n");
	printf("Estos son los comandos disponibles:\n");
	printf("dump vaiables *info: ver las variables compartidas\n");
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

void initSwapConsole(){
	pthread_t newThread;
	pthread_create(&newThread, &nucleo_attr, &consolaDeComandos, NULL);
}
