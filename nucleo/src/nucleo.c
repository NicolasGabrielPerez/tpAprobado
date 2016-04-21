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
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

t_dictionary* create_dictionary_from_strings(char** keys, char** values){
	t_dictionary* dictionary = dictionary_create();
	int i;
	for(i=0 ; keys[i] != NULL ; i++){
		dictionary_put(dictionary, keys[i], values[i]);
	}
	return dictionary;
}

int main(void) {
	t_config* config = config_create("nucleo.config");
    int puerto_prog = config_get_int_value(config, "PUERTO_PROG");
    int puerto_cpu = config_get_int_value(config, "PUERTO_CPU");
    int quantum = config_get_int_value(config, "QUANTUM");
    int quantum_sleep = config_get_int_value(config, "QUANTUM_SLEEP");

	char** io_ids = config_get_array_value(config, "IO_ID");
	char** io_sleep_times = config_get_array_value(config, "IO_SLEEP");

	//para obtener un value, usas (char *)dictionary_get(x_dictionary,clave)
	//OJO, el t_dictionary solo funciona de strings a strings,
	//o sea que para sacar un int, primero lo sacas como char*
	//despues lo podes convertir a int con la funcion atoi()

    t_dictionary* io_dictionary = create_dictionary_from_strings(config, io_ids, io_sleep_times); //lo creo por las dudas

    char** semaforos_ids = config_get_array_value(config, "SEM_ID");
    char** semaforos_init_values = config_get_array_value(config, "SEM_INIT");
    t_dictionary* semaforo_dictionary = create_dictionary_from_strings(semaforos_ids, semaforos_init_values); //lo creo por las dudas...

    char** shared_values = config_get_array_value(config, "SHARED_VARS");

	return EXIT_SUCCESS;
}
