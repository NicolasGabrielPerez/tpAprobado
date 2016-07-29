#include "console-consola.h"
#include "consola.h"
#include <sockets/communication.h>

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

void kill(){
	printf("Comando kill ejecutado\n");

	sendMessage(socket_nucleo, HEADER_FIN_PROGRAMA, 0, "");

	exit(EXIT_SUCCESS);
}

void ejecutarComando(char* command){

	string_trim_left(&command);

	//TODO
	//Hacer los demas comandos y cambiar a str_equals o algo asi.
	if(string_starts_with(command, "kill")){
		kill();
		return;
	}

	printf("Comando invalido\n");
}

void* consolaDeComandos(){
	int rc;

	char* buff = malloc(30);

	while(1){
		rc = getLine ("Esperando comando> ", buff, 30);
		if (rc == NO_INPUT) {
			// Extra NL since my system doesn't output that on EOF.
			printf ("\nSin input de comando\n");
			//free(buff);
			continue;
		}

		if (rc == TOO_LONG) {
			printf ("Comando demasiado largo [%s]\n", buff);
			//free(buff);
			continue;
		}

		ejecutarComando(buff);
		//free(buff);
	}
}

void initCommandConsole(){
	pthread_attr_t pthread_attr;
	pthread_attr_init(&pthread_attr);
	pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED);
	pthread_t newThread;
	pthread_create(&newThread, &pthread_attr, &consolaDeComandos, NULL);
}
