#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <sockets/pcb.h>
#include "cpu-interfaz.h"

typedef struct CPU{
	int socket;
	int disponible;
} CPU;

t_list* CPUs;
t_queue* readys;

void initPlanificador(){
	CPUs = list_create();
	readys = queue_create();
}

void agregarCPU(CPU* cpu){
	list_add(CPUs, cpu);
}

void agregarPCB(PCB* pcb){
	queue_push(readys, pcb);
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

void enviarAEjecutar(PCB* pcb, int cpu_socket){

}

void planificar(){

	while(1){
		if(!queue_is_empty(readys)){ //hay algun PCB para ejecutar
			int cpuDisponible = obtenerCpuDisponible();
			enviarAEjecutar(queue_pop(readys), cpuDisponible);
		}
	}
}

