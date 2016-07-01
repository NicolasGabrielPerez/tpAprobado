#include "cpu.h"

t_list* CPU_control_list;

int CPU_is_free(t_CPU* cpu){
	return cpu->PID == -1;
}

//Devuelve el próximo CPU libre de la lista general
t_CPU* get_next_free_CPU(){
	return list_find(CPU_control_list, CPU_is_free);
}
