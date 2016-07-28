#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <sockets/pcb.h>
#include "cpu-interfaz.h"
#include "planificador.h"

typedef struct{
	int socket;
	int disponible;
} CPU;

t_list* CPUs;
t_queue* ready_status_queue;

void initPlanificador(){
	CPUs = list_create();
	ready_status_queue = queue_create();
}

void agregarCPU(CPU* cpu){
	list_add(CPUs, cpu);
}

void setearDisponible(int cpu_socket){
	int i;
	CPU* actual;
	for(i=0;i<list_size(CPUs);i++){
		actual = list_get(CPUs, i);
		if(actual->socket == cpu_socket){
			actual->disponible = true;
			return;
		}
	}
}

int buscarDisponible(){
	int i;
	CPU* actual;
	for(i=0;i<list_size(CPUs);i++){
		actual = list_get(CPUs, i);
		if(actual->disponible) return actual->socket;
	}

	return NULL;
}

int obtenerCpuDisponible(){
	int cpu_socket = 0;
	while(!cpu_socket){ //mientras que sea NULL el socket...
		cpu_socket = buscarDisponible();
	}
	return cpu_socket;
}

//Al encontrar un cpu libre envía el proximo PCB READY a ejecución
void* plannificationProgram(){
	while(1){
		t_CPU* freeCPU = get_next_free_CPU();
		if(freeCPU != NULL){
			if(!queue_is_empty(READY_Process_Queue)){
				set_next_pcb_RUNNING(freeCPU->cpuSocket);
			}
		}
	}
}
