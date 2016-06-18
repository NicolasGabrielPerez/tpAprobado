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

extern int32_t memoryPageSize;

	void initNucleo(t_config* config);

	//Estructura que representa a un dispositivo de Entrada/Salida
	typedef struct IO_Device{
		char* ioId;
		int sleepTime;
		t_queue* BlockedProcessesQueue;
	} IO_Device;

#endif
