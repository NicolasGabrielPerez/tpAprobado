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

#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>

#include <sockets/sockets.h>

#include "cpu.h"
#include "ansiop.h"
#include "nucleoFunctions.h"
#include "umcFunctions.h"

//static char* DEFINICION_VARIABLES = "variables a, b, c";
//static char* ASIGNACION = "a = b + 12";
//static char* IMPRIMIR = "print b";
//static char* IMPRIMIR_TEXTO = "textPrint foo\n";

t_log* logger;

u_int32_t UMC_PAGE_SIZE = 4;
u_int32_t QUANTUM = 3;

u_int32_t socketNucleo = 0;
u_int32_t socketUmc = 0;

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
		.AnSISOP_wait	= wait,
		.AnSISOP_signal	= signal
};

//Devuelve un booleano si tiene que salir del programa.
bool doQuantum(PCB* pcb, int quantumCount) {
	bool hasToExit = false;

	//Incrementar Program Counter
	pcb->programCounter++;

	//Pedir a la UMC la siguiente instruccion a ejecutar
	char* instruction = "test";//umc_get(codeIndex, offset, size);

	log_trace(logger, string_from_format("Ejecutando quantum: %d", quantumCount));
	log_trace(logger, string_from_format("Ejecutando instruccion: %s", instruction));
	analizadorLinea(strdup(instruction), &functions, &kernel_functions);

	//Notificar al nucleo que concluyo un quantum
	nucleo_notificarFinDeQuantum(quantumCount);

	quantumCount++;

	if(quantumCount >= QUANTUM) {
		//Notificar al nucleo que concluyo una rafaga
		char* result = nucleo_notificarFinDeRafaga(pcb);
		int isDifferent = strcmp(result, "SIGUSR1");
		if(isDifferent == 0) {
			hasToExit = true;
		}

		quantumCount = 0;
	}

	return hasToExit;
}

//Devuelve un booleano si tiene que salir del programa.
bool receiveInstructions(PCB* pcb, int QUANTUM_COUNT) {

	log_trace(logger, "Recibido el PCB, ejecutando...");

	int quantumCounter = 0;
	int hasToExit = false;

	while(quantumCounter <= QUANTUM_COUNT) {
		bool tmp = doQuantum(pcb, quantumCounter);

		if(hasToExit == false) hasToExit = tmp;
		quantumCounter++;
	}
	return hasToExit;
}

void exitProgram() {

	log_trace(logger, "Cerrando programa");

	umc_delete();
	nucleo_delete();

	log_destroy(logger);
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {

	logger = log_create("log.txt", "CPU", true, LOG_LEVEL_TRACE);
//	logger = log_create("log.txt", "CPU", true, LOG_LEVEL_ERROR);

	t_config* config = config_create("cpu.config");
	if(config == NULL){
		log_error(logger, "No se pudo leer la configuración");
		exitProgram();
	}

	log_trace(logger, "Iniciada la configuracion");

	nucleo_init(config);
	umc_init(config);

	bool hasToExit = false;
	while(hasToExit == false) {
		pcb = nucleo_recibir_pcb();
		umc_process_active(pcb->processId);
		hasToExit = receiveInstructions(pcb, QUANTUM);
	}

	exitProgram();

	return EXIT_SUCCESS;
}
