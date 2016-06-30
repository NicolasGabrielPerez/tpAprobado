#ifndef NUCLEO_STRUCTS_H_
#define NUCLEO_STRUCTS_H_
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sockets/sockets.h>
#include <sockets/config.h>
#include <sockets/pcb.h>>

extern int32_t memoryPageSize;
extern t_queue* READY_Process_Queue;
extern t_list* RUNNING_Process_List;
extern t_queue* BLOCKED_Process_Queue;
extern t_list* General_Process_List;

extern t_list* IO_Device_List;

	void initNucleo(t_config* config);

	//Estructura que representa a un dispositivo de Entrada/Salida
	typedef struct IO_Device{
		char* ioId;
		int sleepTime;
		t_queue* BlockedProcessesQueue;
	} t_IO_Device;

	void set_IO_devices_list();

	void set_pcb_READY(PCB* pcb);
	void set_pcb_RUNNING(PCB* pcb);
	void set_pcb_BLOCKED(PCB* pcb);
	void set_pcb_BLOCKED_by_device(PCB* pcb, t_IO_Device* device);

#endif
