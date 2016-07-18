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
#include "test.h"

t_log* logger;
;
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
bool doQuantum(int quantumCount) {
	bool hasToExit = false;

	u_int32_t i = pcb->programCounter;
	u_int32_t start = pcb->codeIndex[i].start;
	u_int32_t size = pcb->codeIndex[i].offset;
	u_int32_t page = start / PAGE_SIZE;
	u_int32_t offset = start % PAGE_SIZE;

	char* instruction = umc_get(page, offset, size);

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

	//Incrementar Program Counter
	pcb->programCounter++;

	return hasToExit;
}

//Devuelve un booleano si tiene que salir del programa.
bool receiveInstructions(PCB* pcb, int quantumCount) {

	log_trace(logger, "Recibido el PCB, ejecutando...");

	int quantumCounter = 0;
	int hasToExit = false;

	while(quantumCounter <= quantumCount) {
		//TODO: A revisar
		if(pcb == 0) return hasToExit;

		bool tmp = doQuantum(quantumCounter);

		if(hasToExit == false) hasToExit = tmp;
		quantumCounter++;
	}
	return hasToExit;
}

void exitProgram() {

	//log_trace(logger, "Cerrando programa");

	umc_delete();
	nucleo_delete();

	log_destroy(logger);
	exit(EXIT_SUCCESS);
}


void test() {

	pcb = test_pcb_init(1);
	umc_process_active(pcb->processId);

	//char* instruction = umc_get(0, 0, 4);

	while(true) {
		char* instruction = "";
		switch(pcb->programCounter) {
			case 0:
				instruction = "begin";
				break;
			case 1:
				instruction = "variables a, b";
				break;
			case 2:
				instruction = "a = 3";
				break;
			case 3:
				instruction = "b = 5";
				break;
			case 4:
				instruction = "a = b + 12";
				break;
			case 5:
				instruction = "end";
				break;
			default:
				free_pcb(pcb);
				exitProgram();
		}
		char* instructionChar = strdup(instruction);
		log_info(logger, "Ejecutando: %s", instructionChar);

		analizadorLinea(instructionChar, &functions, &kernel_functions);
		pcb->programCounter++;

		free(instructionChar);
	}

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

	bool success = nucleo_init(config);
	if(!success) {
		exitProgram();
	}

	//success = umc_init(config);
	if(!success) {
		exitProgram();
	}

//	test();

	bool hasToExit = false;
	while(hasToExit == false) {
		pcb = nucleo_recibir_pcb();
		if(pcb == 0) continue;
		umc_process_active(pcb->processId);
		hasToExit = receiveInstructions(pcb, QUANTUM);
	}

	exitProgram();

	return EXIT_SUCCESS;
}
