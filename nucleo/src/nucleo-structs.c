#include "nucleo-structs.h"

int quantum;
int quantum_sleep;
char** io_ids;
char** io_sleep_times;
char** semaforos_ids;
char** semaforos_init_values;
char** shared_values;
int stack_size;

int32_t memoryPageSize;

t_queue* READY_Process_Queue;
t_queue* BLOCKED_Process_Queue;
t_list* General_Process_List;
t_list* RUNNING_Process_List;

t_list* IO_Device_List;
t_list* CPU_control_list;
t_list* semaforo_control_list;
t_dictionary* vars_control_dictionary;

void initNucleo(t_config* config){
    quantum = config_get_int_value(config, "QUANTUM");
    quantum_sleep = config_get_int_value(config, "QUANTUM_SLEEP");
	io_ids = config_get_array_value(config, "IO_ID");
	io_sleep_times = config_get_array_value(config, "IO_SLEEP");
    semaforos_ids = config_get_array_value(config, "SEM_IDS");
	semaforos_init_values = config_get_array_value(config, "SEM_INIT");
	shared_values = config_get_array_value(config, "SHARED_VARS");
	stack_size = config_get_array_value(config, "STACK_SIZE");

	//Estruturas para control de estados
	READY_Process_Queue = queue_create();
	BLOCKED_Process_Queue = queue_create();
	RUNNING_Process_List = list_create();
	General_Process_List = list_create();

	//Creación de lista de dispositivos
	IO_Device_List = list_create();
	set_IO_devices_list();

	//Lista de control de CPUs conectadas
	CPU_control_list = list_create();

	//Creación de lista de semáforos
	semaforo_control_list = list_create();
	set_semaforo_list();

	//Creación del diccionario de variables
	vars_control_dictionary = dictionary_create();
	set_vars_dictionary();
}

//Devuelve la cantidad de páginas requeridas en SWAP para salvar el código
int getProgramPagesCount(char* program){
	int pagesCount = 0;
	if(memoryPageSize != 0){
		pagesCount =  (strlen(program) % memoryPageSize) + stack_size;
	}

	return pagesCount;
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

void change_status_RUNNING_to_BLOCKED(int PID, char* deviceID){
	PCB* pcb = get_pcb_by_ID(RUNNING_Process_List, deviceID);
	t_IO_Device* device = get_device_by_id(deviceID);
	queue_push(BLOCKED_Process_Queue, device);
}

//Setea la lista general de semáforos con los valores cargados por configuración
void set_semaforo_list(){
	int i = 0;
	while(semaforos_ids[i] != NULL){
		t_semaforo* semaforo = malloc(sizeof(t_semaforo));
		semaforo->sem_id = semaforos_ids[i];
		semaforo->sem_init = semaforos_init_values[i];
		list_add(semaforo_control_list, semaforo);
		i++;
	}
}

//Setea el diccionario general de variables globales definidos por configuración, iniciándolas en cero
void set_vars_dictionary(){
	int i = 0;
		while(shared_values[i] != NULL){
			dictionary_put(vars_control_dictionary, shared_values[i], 0);
		}
}

//Setea el valor de una variable compartida
void set_var_value(char* var_name, int value){
	if(dictionary_has_key(vars_control_dictionary, var_name)){
		dictionary_remove(vars_control_dictionary, var_name);
		dictionary_put(vars_control_dictionary, var_name, value);
	}
}

//Devuelve el valor de la variable var_name
int get_var_value(char* var_name){
	if(dictionary_has_key(vars_control_dictionary, var_name)){
		return dictionary_get(vars_control_dictionary, var_name);
	}
	return 0;
}
