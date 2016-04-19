/*
 ============================================================================
 Name        : nucleo.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
	t_config* config;
	config = config_create("nucleo.config");
    int puerto_prog = config_get_int_value(config, "PUERTO_PROG");
    int puerto_cpu = config_get_int_value(config, "PUERTO_CPU");
    int quantum = config_get_int_value(config, "QUANTUM");
    int quantum_sleep = config_get_int_value(config, "QUANTUM_SLEEP");

    printf("Puerto prog:%d", puerto_prog);
	return EXIT_SUCCESS;
}
