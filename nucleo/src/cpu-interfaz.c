#include "cpu-interfaz.h"

int cpu_listener;

void initCPUListener(t_config* config){
	char* puerto_cpu = config_get_string_value(config, "PUERTO_CPU");
    cpu_listener = crear_puerto_escucha(puerto_cpu);
}
