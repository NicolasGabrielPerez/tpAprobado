#ifndef CPU_INTERFAZ_H_
#define CPU_INTERFAZ_H_
#include "nucleo-structs.h"

	extern fd_set cpu_sockets_set;
	extern int fd_cpu_max;
	extern int cpu_listener;

	void initCPUListener(t_config* config);

#endif
