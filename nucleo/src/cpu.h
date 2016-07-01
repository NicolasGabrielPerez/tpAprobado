#ifndef CPU_H_
#define CPU_H_

#include <commons/collections/list.h>

extern t_list* CPU_control_list;	//Lista general que contiene las referencias a todos los CPUs conectado al módulo

typedef struct CPU{
	int cpu_id;					//Identificador del CPU activo
	int PID;					//Identificador del proceso actual en ejecución
	//TODO: agregar referencia a la conexión con la CPU
} t_CPU;

int CPU_is_free(t_CPU* cpu);

t_CPU* get_next_free_CPU();

#endif /* CPU_H_ */
