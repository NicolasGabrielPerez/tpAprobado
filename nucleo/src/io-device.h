#ifndef IO_DEVICE_H_
#define IO_DEVICE_H_

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
#include <pthread.h>
#include "nucleo-structs.h"

extern t_list* IO_Device_List;

//Estructura que representa a un dispositivo de Entrada/Salida
typedef struct IO_Device{
	char* ioId;
	int sleepTime;
	t_queue* BlockedProcessesQueue;
} t_IO_Device;

void set_IO_devices_list();

void set_pcb_BLOCKED_by_device(PCB* pcb, t_IO_Device* device);
void execute_process_IO(int sleepTime);
void attend_blocked_processes(t_IO_Device* io_device);

t_IO_Device* device_list_find_element(t_list *self, char* ioID, bool(*condition)(void*, int));
int find_device(t_IO_Device* device, char* ioID);
t_IO_Device* get_device_by_id(char* ioID);
int exists_any_blocked_process();
void process_call_io(char* deviceName, int32_t PID);
#endif /* IO_DEVICE_H_ */
