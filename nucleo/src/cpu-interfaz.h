#ifndef CPU_INTERFAZ_H_
#define CPU_INTERFAZ_H_
#include "nucleo-structs.h"
#include <sockets/communication.h>
#include <sockets/serialization.h>

	extern fd_set cpu_sockets_set;
	extern int fd_cpu_max;
	extern int cpu_listener;

	void com_initCPUListener(t_config* config);
	void swichCPU_HEADER(int socket);

	PCB* nucleo_obtener_pcb(message* programBlock);
	void nucleo_notificarIO(char* deviceName, PCB* pcb);

	void* cpu_comunication_program();
	void nucleo_setear_variable(t_globalVar* var);
	void nucleo_notificarFinDeRafaga(message* mensaje, int socket);
	void cpu_sendPCB(PCB* pcb, int cpu_socket);
	void handShakeWithCPU(int cpu_socket);
	void cpu_sendQuantum(int cpu_socket);
	void nucleo_notificarFinDeQuantum(message* mensaje, t_CPU* cpu);

	void nucleo_wait(message* mensaje, t_CPU* cpu);
	void nucleo_signal(message* mensaje);
#endif
