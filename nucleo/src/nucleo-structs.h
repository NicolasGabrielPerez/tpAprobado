#ifndef NUCLEO_STRUCTS_H_
#define NUCLEO_STRUCTS_H_
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/log.h>
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
#include <pthread.h>
#include "io-device.h"
#include "cpu.h"
#include "cpu-interfaz.h"
#include "consola-interfaz.h"
#include "planificador.h"

extern int32_t memoryPageSize;
extern u_int32_t quantum;
extern int quantum_sleep;
extern t_queue* READY_Process_Queue;			//Cola de procesos listos para ejecución
extern t_list* RUNNING_Process_List;			//Lista general de procesos en ejecución
extern t_queue* BLOCKED_Process_Queue;			//Cola general de procesos bloqueados
extern t_list* General_Process_List;			//Lista general de procesos (PCBs)
extern t_list* CPU_control_list;				//Lista general que contiene las referencias a todos los CPUs conectado al módulo
extern t_list* semaforo_control_list;			//Lista general de semáforos dedicado a operaciones privilegiadas
extern t_dictionary* vars_control_dictionary;	//Diccionario general de variables globales
extern char* umc_ip;

extern char** io_ids;
extern char** io_sleep_times;

extern int test_mode;
extern int io_thread_sleep;

extern t_log* nucleo_logger;

typedef struct semaforo{
	char* sem_id;					//Nombre del semáforo
	u_int32_t sem_value;			//Valor
	t_queue* blocked_process_queue;	//Cola de procesos bloqueados
} t_semaforo;

void initNucleo(t_config* config);
void set_IO_devices_list();
void set_pcb_READY(PCB* pcb);
void set_pcb_RUNNING(PCB* pcb);
void set_pcb_BLOCKED(PCB* pcb);
void set_pcb_EXIT(int processID);

int getProgramPagesCount(char* program);

void end_process(int PID);

void add_pcb_to_general_list(PCB* pcb);

void nucleo_updatePCB(PCB* pcb);

void change_status_RUNNING_to_READY(t_CPU* cpu);

void initNewProgram(u_int32_t codeSize, char* programSourceCode, int consoleSocket);

void set_var_value(char* var_name, int value);
void hiloDeLectura(t_config* config);
void set_next_pcb_RUNNING(int cpu_id);
int wait(char* id);
void signal(char* id);
void set_pcb_READY(PCB* pcb);

void init_threads_config(pthread_attr_t nucleo_attr);
void init_cpu_communication_thread(pthread_attr_t nucleo_attr);
void init_console_communication_thread(pthread_attr_t nucleo_attr);

t_semaforo* get_semaforo_by_ID(t_list* list, char* id);
void queue_blocked_process_to_semaforo(char* id, PCB* pcb);

void nucleo_nuevo_cpu(int cpu_socket);
#endif
