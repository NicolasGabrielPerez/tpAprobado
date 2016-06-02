#include "consola-interfaz.h"

int consola_listener;

void initConsolaListener(t_config* config){
	char* puerto_prog = config_get_string_value(config, "PUERTO_PROG");
	consola_listener = crear_puerto_escucha(puerto_prog);
}

