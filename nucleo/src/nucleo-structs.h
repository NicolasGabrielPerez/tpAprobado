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
#include <sockets/pcb.h>
#include "io-device.h"

extern int32_t memoryPageSize;
extern t_queue* READY_Process_Queue;			//Cola de procesos listos para ejecución
extern t_list* RUNNING_Process_List;			//Lista general de procesos en ejecución
extern t_queue* BLOCKED_Process_Queue;			//Cola general de procesos bloqueados
extern t_list* General_Process_List;			//Lista general de procesos (PCBs)
extern t_list* CPU_control_list;				//Lista general que contiene las referencias a todos los CPUs conectado al módulo
extern t_list* semaforo_control_list;			//Lista general de semáforos dedicado a operaciones privilegiadas
extern t_dictionary* vars_control_dictionary;	//Diccionario general de variables globales

extern char** io_ids;
extern char** io_sleep_times;

void initNucleo(t_config* config);
void set_IO_devices_list();
void set_pcb_READY(PCB* pcb);
void set_pcb_RUNNING(PCB* pcb);
void set_pcb_BLOCKED(PCB* pcb);

int getProgramPagesCount(char* program);

typedef struct semaforo{
	char* sem_id;					//Nombre del semáforo
	u_int32_t sem_value;			//Valor
	t_queue* blocked_process_queue;	//Cola de procesos bloqueados
} t_semaforo;

#endif
