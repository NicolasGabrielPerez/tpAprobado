#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>

int main(void) {
	t_config* config = config_create("swap.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}
	int puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
	int cantidad_de_paginas = config_get_int_value(config, "CANTIDAD_PAGINAS");
	int tamanio_de_pagina = config_get_int_value(config, "TAMANIO_PAGINA");
	int retardo_compactacion = config_get_int_value(config, "RETARDO_COMPACTACION");
	return EXIT_SUCCESS;
}
