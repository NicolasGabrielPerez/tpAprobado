#include "cpu.h"

t_list* CPU_control_list;

t_CPU* new_cpu(int cpu_id){
	t_CPU* cpu = malloc(sizeof(t_CPU));
	cpu->cpuSocket = cpu_id;
	cpu->PID = -1;

	return cpu;
}

void add_new_cpu(t_CPU* cpu){
	list_add(CPU_control_list, cpu);
}
//Valida que una CPU esté libre
int CPU_is_free(t_CPU* cpu){
	return cpu->PID == -1;
}

int free_CPU(t_CPU* cpu, int socket){
	return cpu->cpuSocket == socket;
}

//Valida que el identificador de CPU sea cpu_id (cpu_id contiene el socket de la conexión al CPU)
int find_CPU(t_CPU* cpu, int cpu_id){
	return cpu->cpuSocket == cpu_id;
}

//Función auxiliar para traer un CPU de la lista general por ID
t_CPU* cpu_list_find_element(t_list *self, int PID, bool(*condition)(void*, int)) {
	t_link_element *element = list_find_by_id(self, PID,condition, NULL);
	return element != NULL ? element->data : NULL;
}

//Devuelve el próximo CPU libre de la lista general, si no NULL
t_CPU* get_next_free_CPU(){
	return list_find(CPU_control_list, CPU_is_free);
}

//Devuelve la CPU conectada a través de socket
t_CPU* get_CPU_by_socket(int socket){
	return cpu_list_find_element(CPU_control_list, socket, find_CPU);
}

//Remplaza el valor de proceso en ejecución asociado a una cpu
void liberarCpu(int socket){
	t_CPU* unaCpu = get_CPU_by_socket(socket);
	unaCpu->PID = -1;
}

void enviar_a_cpu_libre(int PID, char* serializado){
	 int contenidoSize = sizeof(serializado);
	 t_CPU* cpuLibre = get_next_free_CPU();
	 cpuLibre->PID = PID;
	 int sockete = cpuLibre->cpuSocket;
	 int enviarOKConContenido(sockete, contenidoSize, serializado);
}
