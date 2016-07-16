#include "console-umc.h"

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

void parsearRetardo(char* command){

	int offset = 7; //"retardo"
	char* sRetardo = string_substring_from(command, offset);
	int retardo = strtol(sRetardo, NULL, 10);

	if(retardo<=0){
		printf("Tiempo de retardo invalido\n");
		return;
	}

	setRetardo(retardo);
}

void parsearDumpTable(char* command){
	int offset = sizeof("dump table") - 1;
	char* sPid = string_substring_from(command, offset);

	int pid = strtol(sPid, NULL, 10);

	if(pid <= 0){
		dumpAllTables();
		return;
	}

	dumpTable(pid);
}

void parsearDumpData(char* command){
	int offset = sizeof("dump data") - 1;
	char* sPid = string_substring_from(command, offset);

	int pid = strtol(sPid, NULL, 10);

	if(pid <= 0){
		dumpAllData();
		return;
	}

	dumpData(pid);
}

void parsearFlushMemory(char* command){
	int offset = sizeof("dump data") - 1;
	char* sPid = string_substring_from(command, offset);

	int pid = strtol(sPid, NULL, 10);

	if(pid <= 0){
		printf("Fallo: pid invalido o vacio");
		return;
	}

	flushMemory(pid);
}

void ejecutarComando(char* command){
	string_trim_left(&command);

	if(string_starts_with(command, "retardo")){
		parsearRetardo(command);
		return;
	}
	if(string_starts_with(command, "dump table")){
		parsearDumpTable(command);
		return;
	}
	if(string_starts_with(command, "dump data")){
		parsearDumpData(command);
		return;
	}
	if(string_starts_with(command, "flush tlb")){
		flushTlb();
		return;
	}
	if(string_starts_with(command, "flush memory")){
		parsearFlushMemory(command);
		return;
	}

	printf("Comando invalido\n");
}

void* consolaDeComandos(){
	printf("--------------- Bienvenido a la consola de UMC ---------------\n");
	printf("Estos son los comandos disponibles:\n");
	printf("retardo <milisegundos> *Retardo a esperar ante una solicitud\n");
	printf("dump <info> <pid> *info: 'table' o 'data'. Pid vacio => todos los procesos.\n");
	printf("flush tlb *Limpiar toda la tlb\n");
	printf("flush memory <pid> *Marca como modificadas todas las paginas del pid\n");
	printf("----------------------------------------------------------------------------\n");

	int rc;
	char* buff = malloc(30);

	while(1){
		rc = getLine ("Esperando comando> ", buff, 30);
		if (rc == NO_INPUT) {
			// Extra NL since my system doesn't output that on EOF.
			printf ("\nSin input de comando\n");
			continue;
		}

		if (rc == TOO_LONG) {
			printf ("Comando demasiado largo [%s]\n", buff);
			continue;
		}

		ejecutarComando(buff);
		buff = malloc(30);
	}
}

void initUmcConsole(){
	pthread_t newThread;
	pthread_create(&newThread, &nucleo_attr, &consolaDeComandos, NULL);
}
