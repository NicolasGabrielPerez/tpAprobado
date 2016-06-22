#include "nucleo-structs.h"

int quantum;
int quantum_sleep ;
char** io_ids;
char** io_sleep_times;
int32_t memoryPageSize;
t_queue* READY_Process_Queue;
t_queue* BLOCKED_Process_Queue;
t_queue* General_Process_Queue;

t_list* IO_Device_List;


/*
- Crear cola de READY
- Crear cola de Bloqueados
- Crear cola de general de Procesos
- Crear cola de Bloqueados por cada dispositivo E/S
*/

void initNucleo(t_config* config){
    quantum = config_get_int_value(config, "QUANTUM");
    quantum_sleep = config_get_int_value(config, "QUANTUM_SLEEP");
	io_ids = config_get_array_value(config, "IO_ID");
	io_sleep_times = config_get_array_value(config, "IO_SLEEP");
    //char** semaforos_ids = config_get_array_value(config, "SEM_ID");
	//char** semaforos_init_values = config_get_array_value(config, "SEM_INIT");
	//char** shared_values = config_get_array_value(config, "SHARED_VARS");

	READY_Process_Queue = queue_create();
	BLOCKED_Process_Queue = queue_create();
	General_Process_Queue = queue_create();

	//Creación de lista de dispositivos
	IO_Device_List = list_create();
	int i = 0;
	while(io_ids[i] != NULL){
		IO_Device *device = malloc(sizeof(IO_Device));
		device->ioId = io_ids[i];
		device->sleepTime = io_sleep_times[i];
		device->BlockedProcessesQueue = queue_create();

		list_add(IO_Device_List, device);
		i++;
	}
	//TODO: debo liberar la memoria de los arrays de confifuración????
}

int getProgramPagesCount(char* program){
	int pagesCount = 0;
	if(memoryPageSize != 0){
		pagesCount =  strlen(program) % memoryPageSize;
	}

	return pagesCount;
}
