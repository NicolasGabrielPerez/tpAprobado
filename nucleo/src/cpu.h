#ifndef CPU_H_
#define CPU_H_

#include <commons/collections/list.h>
#include <sockets/communication.h>
#include <sockets/pcb.h>

typedef struct CPU{
	int cpuSocket;				//Identificador del CPU activo => referencia a la conexión
	//int consoleSocket;			//Identificador de la consola que inició el proceso
	int PID;					//Identificador del proceso actual en ejecución
} t_CPU;

int CPU_is_free(t_CPU* cpu);

t_CPU* get_next_free_CPU();
t_CPU* get_CPU_by_socket(int socket);
t_CPU* new_cpu(int cpu_id);
void add_new_cpu(t_CPU* cpu);

#endif /* CPU_H_ */
