#include "cpu.h"
#include <sockets/communication.h>

t_list* CPU_control_list;

int CPU_is_free(t_CPU* cpu){
	return cpu->PID == -1;
}

int free_CPU(t_CPU* cpu, int socket){
	return cpu->socket == socket;
}

//Devuelve el próximo CPU libre de la lista general
t_CPU* get_next_free_CPU(){
	return list_find(CPU_control_list, CPU_is_free);
}

t_CPU* get_CPU_whit_sock(int socket){

	return list_find(CPU_control_list, free_CPU);
}

void liberarCpu(int socket){
	t_CPU* unaCpu = get_CPU_whit_sock(socket);
	unaCpu->PID = -1;
}


void enviar_a_cpu_libre(int PID, char* serializado){
	 int contenidoSize = sizeof(serializado);
	 t_CPU* cpuLibre =get_next_free_CPU();
	 cpuLibre->PID = PID;
	 int sockete = cpuLibre->socket;
	int enviarOKConContenido(sockete, contenidoSize, serializado);

}
