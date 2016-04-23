/*
 ============================================================================
 Name        : CPU.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include <commons/log.h>
#include <commons/collections/list.h>

#include "ansiop.h"
#include "configInit.h"

#include <librery.h>

static const char* DEFINICION_VARIABLES = "variables a, b, c";
static const char* ASIGNACION = "a = b + 12";
static const char* IMPRIMIR = "print b";
static const char* IMPRIMIR_TEXTO = "textPrint foo\n";

int socketNucleo = 0;
int socketUmc = 0;

AnSISOP_funciones functions = {
	.AnSISOP_definirVariable	= definirVariable,
	.AnSISOP_obtenerPosicionVariable= obtenerPosicionVariable,
	.AnSISOP_dereferenciar	= dereferenciar,
	.AnSISOP_asignar	= asignar,
	.AnSISOP_imprimir	= imprimir,
	.AnSISOP_imprimirTexto	= imprimirTexto,
};
AnSISOP_kernel kernel_functions = { };


void correrDefinirVariables() {
	printf("Ejecutando '%s'\n", DEFINICION_VARIABLES);
	analizadorLinea(strdup(DEFINICION_VARIABLES), &functions, &kernel_functions);
	printf("================\n");
}


void correrAsignar() {
	printf("Ejecutando '%s'\n", ASIGNACION);
	analizadorLinea(strdup(ASIGNACION), &functions, &kernel_functions);
	printf("================\n");
}


void correrImprimir() {
	printf("Ejecutando '%s'\n", IMPRIMIR);
	analizadorLinea(strdup(IMPRIMIR), &functions, &kernel_functions);
	printf("================\n");
}



void correrImprimirTexto() {
	printf("Ejecutando '%s'", IMPRIMIR_TEXTO);
	analizadorLinea(strdup(IMPRIMIR_TEXTO), &functions, &kernel_functions);
	printf("================\n");
}

void socketInit() {

	int resultNucleo = crear_cliente(&socketNucleo, NUCLEO_IP, NUCLEO_PORT);
	int resultUmc = crear_cliente(&socketUmc, UMC_IP, UMC_PORT);

	if(resultNucleo == 0) {
		printf("Bien papa nucleo\n");
		printf("Result: %d\n", socketNucleo);
	} else {
		printf("Mal papa nucleo\n");
	}

	if(resultUmc == 0) {
		printf("Bien papa umc\n");
		printf("Result: %d\n", socketUmc);
	} else {
		printf("Mal papa umc\n");
	}

}

void sendMessage() {
	char package[PACKAGESIZE];
	int enviar = 1;

	while(enviar){
			char* receivedData = fgets(package, PACKAGESIZE, stdin);
			printf("CPU: Mensaje Recibido\n");
			if (!strcmp(package,"exit\n")) enviar = 0;
			if (enviar) {
				send(socketNucleo, package, PACKAGESIZE, 0);
				//send(socketNucleo, package, strlen(package) + 1, 0);
			}
	}
}

int main(int argc, char **argv) {
	//correrDefinirVariables();
	//correrAsignar();
	//correrImprimir();
	//correrImprimirTexto();
	//initConfig();

	socketInit();
	sendMessage();

	return 0;
}
