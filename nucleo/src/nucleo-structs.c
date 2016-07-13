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

//Redefinición de método para buscar dentro de una lista por nombre
t_link_element* list_find_by_id_char(t_list *self, char* ID, bool(*condition)(void*, int), int* index) {
	t_link_element *element = self->head;
	int position = 0;

	while (element != NULL && !condition(element->data, ID)) {
		element = element->next;
		position++;
	}

	if (index != NULL) {
		*index = position;
	}

	return element;
}

//---------------------------------------------- <SEMÁFOROS>

//Redefinición de método. Devuelve un PCB según PID
t_semaforo* semaforo_list_find_element(t_list *self, char* id, bool(*condition)(void*, int)) {
	t_link_element *element = list_find_by_id(self, id,condition, NULL);
	return element != NULL ? element->data : NULL;
}

//Función auxiliar
int find_semaforo(t_semaforo* semaforo, char* id){
	return semaforo->sem_id == id;
}

//Devuelve un semaforo con id = id dentro de list
t_semaforo* get_semaforo_by_ID(t_list* list, char* id){
	return semaforo_list_find_element(list, id, find_semaforo);
}

//Setea la lista general de semáforos con los valores cargados por configuración
void set_semaforo_list(){
	int i = 0;
	while(semaforos_ids[i] != NULL){
		t_semaforo* semaforo = malloc(sizeof(t_semaforo));
		semaforo->sem_id = semaforos_ids[i];
		semaforo->sem_value = (u_int32_t)semaforos_init_values[i];
		semaforo->blocked_process_queue = queue_create();
		list_add(semaforo_control_list, semaforo);
		i++;
	}
}

//Encola un proceso en la cola de bloqueados de un semáforo
void queue_blocked_process_to_semaforo(char* id, PCB* pcb){
	t_semaforo* semaforo = get_semaforo_by_ID(semaforo_control_list, id);
	queue_push(semaforo->blocked_process_queue, pcb);
}

//Instrucción privilegiada
void signal(char* id){
	t_semaforo* semaforo = get_semaforo_by_ID(semaforo_control_list, id);
	semaforo->sem_value ++;

	if(!queue_is_empty(semaforo->blocked_process_queue)){
		//Poner en cola de ready al próximo proceso bloqueado
		set_pcb_READY(queue_pop(semaforo->blocked_process_queue));
	}
}

//Instrucción privilegiada
void wait(char* id){
	t_semaforo* semaforo = get_semaforo_by_ID(semaforo_control_list, id);
	if(semaforo->sem_value > 0){
		semaforo->sem_value --;
	}
	else{
		//TODO: GUTI - implementar
		//Bloquear proceso => desalojar de CPU
		//Disparar la rutina de cambio de contexto
	}
}
//---------------------------------------------- </SEMÁFOROS>

//---------------------------------------------- <PCBs>

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

//Finalizar un proceso
void end_process(int PID){

}
//---------------------------------------------- </PCBs>

//---------------------------------------------- <VARIABLES>
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
//---------------------------------------------- </VARIABLES>
