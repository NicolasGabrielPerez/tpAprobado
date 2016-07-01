#ifndef CPU_H_
#define CPU_H_

#include <commons/collections/list.h>
#include <sockets/communication.h>
#include <sockets/pcb.h>

typedef struct CPU{
	int socket;					//Identificador del CPU activo => referencia a la conexión
	int PID;					//Identificador del proceso actual en ejecución
} t_CPU;

int CPU_is_free(t_CPU* cpu);

t_CPU* get_next_free_CPU();

#endif /* CPU_H_ */
