#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <parser/metadata_program.h>
#include <parser/parser.h>
#include <parser/sintax.h>

#include <signal.h>
#include <pthread.h>

#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>

#include <sockets/sockets.h>

#include "cpu.h"
#include "ansiop.h"
#include "nucleoFunctions.h"
#include "umcFunctions.h"
#include "test.h"

t_log* logger;

u_int32_t QUANTUM = 3;
u_int32_t quantum = 0;
u_int32_t SLEEP_QUANTUM = 1;

u_int32_t socketNucleo = 0;
u_int32_t socketUmc = 0;

bool hasToExitCPU = false;
bool hasToReturn = false;
bool hasToFinishProgram = false;

AnSISOP_funciones functions = {
	.AnSISOP_definirVariable	= definirVariable,
	.AnSISOP_obtenerPosicionVariable= obtenerPosicionVariable,
	.AnSISOP_dereferenciar	= dereferenciar,
	.AnSISOP_asignar	= asignar,
	.AnSISOP_obtenerValorCompartida = obtenerValorCompartida,
	.AnSISOP_asignarValorCompartida = asignarValorCompartida,
	.AnSISOP_irAlLabel = irAlLabel,
	.AnSISOP_llamarSinRetorno = llamarSinRetorno,
	.AnSISOP_llamarConRetorno = llamarConRetorno,
	.AnSISOP_finalizar = finalizar,
	.AnSISOP_retornar = retornar,
	.AnSISOP_imprimir = imprimir,
	.AnSISOP_imprimirTexto	= imprimirTexto,
	.AnSISOP_entradaSalida	= entradaSalida
};

AnSISOP_kernel kernel_functions = {
		.AnSISOP_wait	= ansiop_wait,
		.AnSISOP_signal	= ansiop_signal
};


void sigusr1_handler(int signum) {
    if (signum == SIGUSR1)
    {
    	log_trace(logger, string_from_format("Recibida señal SIGUSR1... Saliendo del programa"));
        hasToExitCPU = true;
    }
}


void doQuantum() {

	if(pcb == 0) return;

	u_int32_t i = pcb->programCounter;
	u_int32_t start = pcb->codeIndex[i].start;
	u_int32_t size = pcb->codeIndex[i].offset;

	char* instruction = umc_get_with_page_control(start, size);

	log_trace(logger, "Ejecutando quantum: %d", quantum);
	log_trace(logger, "Ejecutando instruccion: %s", instruction);

	if(instruction == 0 || strlen(instruction) < 2) {

		nucleo_imprimir_texto("Fin de programa por error de memoria en la instruccion");
		nucleo_notificarFinDePrograma();
		return;
	}

	char* instructionDupped = strdup(instruction);

	analizadorLinea(instructionDupped, &functions, &kernel_functions);
	usleep(SLEEP_QUANTUM);

	free(instruction);
	free(instructionDupped);

	if(pcb == 0) return;

	//Notificar al nucleo que concluyo un quantum
	nucleo_notificarFinDeQuantum(quantum);

	quantum++;

	//Incrementar Program Counter
	pcb->programCounter++;

	if(hasToFinishProgram) {
		nucleo_imprimir_texto("Finalizando programa debido a un acceso no valido a memoria o falta memoria.");
		nucleo_notificarFinDePrograma();

		hasToReturn = true;
		return;
	}


	if(quantum >= QUANTUM) {
		//Notificar al nucleo que concluyo una rafaga
		nucleo_notificarFinDeRafaga(pcb);

		quantum = 0;
	}

	return;
}

//Devuelve un booleano si tiene que salir del programa.
void receiveInstructions(int quantumCount) {

	log_trace(logger, "Recibido el PCB, ejecutando...");

	int quantumCounter = 0;

	while(quantumCounter < quantumCount) {
		if(pcb == 0) return;

		doQuantum(quantumCounter);

		if(pcb == 0) return;

		if(hasToReturn) {
			hasToReturn = false;
			hasToFinishProgram = false;
			return;
		}

		quantumCounter++;
	}
}

void exitProgram() {

	//log_trace(logger, "Cerrando programa");

	umc_delete();
	nucleo_delete();

	log_destroy(logger);
	exit(EXIT_SUCCESS);
}

void test() {
	printf("Iniciado thread\n");
	signal(SIGUSR1, sigusr1_handler);
}

pthread_attr_t pthread_attr;
void createSIGUSR1Thread() {
	pthread_t thread;
	pthread_create(&thread, &pthread_attr, &test,  0);
}

int lastProcessId = -1;
int main(int argc, char **argv) {

	logger = log_create("log.txt", "CPU", true, LOG_LEVEL_TRACE);
//	logger = log_create("log.txt", "CPU", true, LOG_LEVEL_ERROR);

	t_config* config = config_create("cpu.config");
	if(config == NULL){
		log_error(logger, "No se pudo leer la configuración");
		exitProgram();
	}

	log_trace(logger, "Iniciada la configuracion");

	bool success;

	success = nucleo_init(config);
	if(!success) {
		exitProgram();
	}

	success = umc_init(config);
	if(!success) {
		exitProgram();
	}

	createSIGUSR1Thread();

	while(hasToExitCPU == false) {
		pcb = nucleo_recibir_pcb();
		if(pcb == 0) continue;

		quantum = 0;
		hasToReturn = false;

		if(lastProcessId != pcb->processId) {
			lastProcessId = pcb->processId;
			umc_process_active(pcb->processId);
		}
		receiveInstructions(QUANTUM);
	}

	exitProgram();

	return EXIT_SUCCESS;
}
