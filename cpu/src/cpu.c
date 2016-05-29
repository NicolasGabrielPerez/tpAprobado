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
#include <commons/log.h>
#include <commons/collections/list.h>

#include "sockets.h"

#include "ansiop.h"
#include "nucleoFunctions.h"
#include "umcFunctions.h"

static char* DEFINICION_VARIABLES = "variables a, b, c";
static char* ASIGNACION = "a = b + 12";
static char* IMPRIMIR = "print b";
static char* IMPRIMIR_TEXTO = "textPrint foo\n";

u_int32_t UMC_PAGE_SIZE = 4;
u_int32_t QUANTUM_SIZE = 3;

u_int32_t quantumCount = 0;


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
int doQuantum(PCB* pcb) {

	int hasToExit = 0;

	//Incrementar Program Counter
	pcb->programCounter++;

	//Pedir a la UMC la siguiente instruccion a ejecutar
	char* instruction;

	//test
	switch(quantumCount) {
		case 0:
			instruction = DEFINICION_VARIABLES;
			break;
		case 1:
			instruction = ASIGNACION;
			break;
		case 2:
			instruction = IMPRIMIR;
			break;
		case 3:
			instruction = IMPRIMIR_TEXTO;
			break;
	}

	analizadorLinea(strdup(instruction), &functions, &kernel_functions);

	//Notificar al nucleo que concluyo un quantum
	nucleo_notificarFinDeQuantum(quantumCount);

	quantumCount++;

	if(quantumCount >= QUANTUM_SIZE) {
		//Notificar al nucleo que concluyo una rafaga
		nucleo_notificarFinDeRafaga(pcb);

		quantumCount = 0;
	}

	return hasToExit;
}


/*
int main(int argc, char **argv) {

	t_config* config = config_create("cpu.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}

	char buf[50];
	int nbytes;

	char* puerto_nucleo = config_get_string_value(config, "PUERTO_NUCLEO");
	char* puerto_umc = config_get_string_value(config, "PUERTO_UMC");

	printf("Config: PUERTO_NUCLEO=%s\n", puerto_nucleo);
	printf("Config: PUERTO_UMC=%s\n", puerto_umc);

	int socket_umc = crear_socket_cliente("utnso40", puerto_umc); //socket usado para conectarse a la umc
	int socket_nucleo = crear_socket_cliente("utnso40", puerto_nucleo); //socket usado para conectarse a la umc

	printf("UMC FD: %d\n", socket_umc);
	printf("NUCLEO FD: %d\n", socket_nucleo);

	//Hago handshake con umc
	if(handshake(socket_umc, "PRUEBA") != 0){
		puts("Error en handshake con la umc");
	}

	//Hago handskae con nucleo
	if(handshake(socket_nucleo, "PRUEBA") != 0){
		puts("Error en handshake con la umc");
	}

	//Quiero recibir de núcleo, lo que le pasó consola
	if ((nbytes = recv(socket_nucleo, buf, 50, 0)) <= 0) {
	   // got error or connection closed by client
	   if (nbytes == 0) {
		   // connection closed
		   printf("socket %d hung up\n", socket_nucleo);
	   } else {
		   perror("recv");
	   }
	   close(socket_nucleo); // bye!
   } else {
	   //se recibió mensaje
	   printf("Se recibieron %d bytes\n", nbytes);
	   printf("Se recibió: %s\n", buf);

  } // END handle data from client


	if (send(socket_umc, buf, nbytes, 0) == -1) { //envio lo mismo que me acaba de llegar => misma cant de bytes a enviar
		 perror("send");
    };

	puts("Ya le hice un envio a UMC. Termino mi ejecucion.");

	return EXIT_SUCCESS;

}*/

int main(int argc, char **argv) {

	t_config* config = config_create("cpu.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}

	//nucleo_init(config);
	umc_init(config);

	umc_process_active(10);

/*	int hasToExit = 0;
	while(hasToExit == 0) {
		pcb = nucleo_recibirInstruccion();
		hasToExit = doQuantum(pcb);
	}*/

	umc_delete();
	//nucleo_delete();

	return EXIT_SUCCESS;
}
