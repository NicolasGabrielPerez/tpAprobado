#include "cpu-interfaz.h"

int cpu_listener;
fd_set cpu_sockets_set;
int fd_cpu_max;

void initCPUListener(t_config* config){
	char* puerto_cpu = config_get_string_value(config, "PUERTO_CPU");
    cpu_listener = crear_puerto_escucha(puerto_cpu);
    FD_ZERO(&cpu_sockets_set);    // clear the master and temp sets
	FD_SET(cpu_listener, &cpu_sockets_set);
}
