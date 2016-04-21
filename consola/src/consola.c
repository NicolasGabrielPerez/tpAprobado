/*
 ============================================================================
 Name        : consola.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>

int main(void) {
	t_config* config = config_create("consola.config");
	t_config* config = config_create("nucleo.config");
		if(config==NULL){
			printf("No se pudo leer la configuración");
			return EXIT_FAILURE;
		}
    int puerto = config_get_int_value(config, "PUERTO");
    int ip_swap = config_get_int_value(config, "IP_SWAP");
    int puerto_swap = config_get_int_value(config, "PUERTO_SWAP");
    int cantdiad_de_marcos = config_get_int_value(config, "MARCOS");
    int marco_size = config_get_int_value(config, "MARCO_SIZE");
    int ip_marcos_x_proc = config_get_int_value(config, "MARCOS_X_PROC");
    int cantidad_entradas_tlb = config_get_int_value(config, "ENTRADAS_TLB");
    int retardo = config_get_int_value(config, "RETARDO");

    printf("Puerto:%d", puerto);
	return EXIT_SUCCESS;
}
