#include "nucleo-structs.h"

int quantum;
int quantum_sleep ;
char** io_ids;
char** io_sleep_times;

int32_t memoryPageSize;
t_queue* READY_Process_Queue;
t_queue* BLOCKED_Process_Queue;
t_list* General_Process_List;
t_list* RUNNING_Process_List;

t_list* IO_Device_List;

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
	RUNNING_Process_List = list_create();
	General_Process_List = list_create();

	//Creación de lista de dispositivos
	IO_Device_List = list_create();
	set_IO_devices_list();
}

int getProgramPagesCount(char* program){
	int pagesCount = 0;
	if(memoryPageSize != 0){
		pagesCount =  strlen(program) % memoryPageSize;
	}

	return pagesCount;
}

//Levanta los dispositivos externos desde configuración y arma una lista de t_IO_Device
void set_IO_devices_list(){
	t_list* deviceList = list_create();
	int i = 0;
	while(io_ids[i] != NULL){
		t_IO_Device* device = malloc(sizeof(t_IO_Device));
		device->ioId = io_ids[i];
		device->sleepTime = io_sleep_times[i];
		device->BlockedProcessesQueue = queue_create();
		list_add(IO_Device_List, device);
		i++;
	}
	//TODO: debo liberar la memoria de los arrays de configuración????
}

//Agrega pcb a la lista general de procesos
void add_pcb_to_general_list(PCB* pcb){
	list_add(General_Process_List, pcb);
}

//Encola pcb en la cola general de listos
void set_pcb_READY(PCB* pcb){
	queue_push(READY_Process_Queue, pcb);
}
//Encola pcb en la cola general de Running
void set_pcb_RUNNING(PCB* pcb){
	list_add(RUNNING_Process_List, pcb);
}

//Encola pcb en la cola general de bloqueados
void set_pcb_BLOCKED(PCB* pcb){
	queue_push(BLOCKED_Process_Queue, pcb);
}

//Encola pcb en la cola de bloqueados de device
void set_pcb_BLOCKED_by_device(PCB* pcb, t_IO_Device* device){
	queue_push(device->BlockedProcessesQueue, pcb);
}

void change_status_RUNNING_to_READY(int PID){
	//traer PCB de RUNNING
	PCB* pcb = get_pcb_by_ID(RUNNING_Process_List,  PID);
	//encolar en ready
	set_pcb_READY(pcb);
	//sacar de lista de running
	pcb = remove_pcb_by_ID(RUNNING_Process_List, PID);

	if(pcb != NULL){
		//PCB removido
	}
}
