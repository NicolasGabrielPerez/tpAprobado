#include "nucleo-structs.h"

u_int32_t quantum;
int quantum_sleep;
char** io_ids;
char** io_sleep_times;
char** semaforos_ids;
char** semaforos_init_values;
char** shared_values;
int stack_size;
int test_mode;
int io_thread_sleep;
char* umc_ip;

int32_t memoryPageSize;

t_queue* READY_Process_Queue;
t_queue* BLOCKED_Process_Queue;
t_list* General_Process_List;
t_list* RUNNING_Process_List;

t_list* IO_Device_List;
t_list* CPU_control_list;
t_list* semaforo_control_list;
t_dictionary* vars_control_dictionary;

u_int32_t quantum;

t_log* nucleo_logger;

void initNucleo(t_config* config){
	quantum = config_get_int_value(config, "QUANTUM");
	quantum_sleep = config_get_int_value(config, "QUANTUM_SLEEP");
	io_ids = config_get_array_value(config, "IO_ID");
	io_sleep_times = config_get_array_value(config, "IO_SLEEP");
	semaforos_ids = config_get_array_value(config, "SEM_IDS");
	semaforos_init_values = config_get_array_value(config, "SEM_INIT");
	shared_values = config_get_array_value(config, "SHARED_VARS");
	stack_size = config_get_int_value(config, "STACK_SIZE");
	umc_ip = config_get_string_value(config, "UMC_IP");
	test_mode = config_get_int_value(config, "TEST_MODE");
	io_thread_sleep = config_get_int_value(config, "IO_THREAD_SLEEP");


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
	t_semaforo* semaforo;
	int i;
	for(i = 0 ; i < list_size(semaforo_control_list) ; i++){
		semaforo = list_get(semaforo_control_list, i);
		if(!strcmp(semaforo->sem_id, id)){
			return semaforo;
		}
	}
	return NULL;
}

//Setea la lista general de semáforos con los valores cargados por configuración
void set_semaforo_list(){
	int i = 0;
	while(semaforos_ids[i] != NULL){
		t_semaforo* semaforo = malloc(sizeof(t_semaforo));
		semaforo->sem_id = semaforos_ids[i];
		semaforo->sem_value = atoi(semaforos_init_values[i]);
		semaforo->blocked_process_queue = queue_create();
		list_add(semaforo_control_list, semaforo);
		i++;
	}
}

//Encola un proceso en la cola de bloqueados de un semáforo
void queue_blocked_process_to_semaforo(char* id, PCB* pcb){
	t_semaforo* semaforo = get_semaforo_by_ID(semaforo_control_list, id);
	queue_push(semaforo->blocked_process_queue, pcb);
	log_trace(nucleo_logger, "PLANIFICACION: Proceso %d bloqueado por semáforo %s", pcb->processId, semaforo->sem_id);
}

//Instrucción privilegiada
void signal(char* id){
	t_semaforo* semaforo = get_semaforo_by_ID(semaforo_control_list, id);
	semaforo->sem_value ++;
	PCB* nextPcb;

	//TODO: Guti - TESTEAR
	if(!queue_is_empty(semaforo->blocked_process_queue)){
		//Poner en cola de ready al próximo proceso bloqueado
		nextPcb = queue_pop(semaforo->blocked_process_queue);

		if(is_program_alive(nextPcb->processId)){
			set_pcb_READY(nextPcb);
		}
		else{
			while(!queue_is_empty(semaforo->blocked_process_queue)){
				if(!is_program_alive(nextPcb->processId)){
					free_pcb(nextPcb);
					nextPcb = queue_pop(semaforo->blocked_process_queue);
				}
				else{
					set_pcb_READY(nextPcb);
					break;
				}
			}
		}
	}
}

//Instrucción privilegiada
int wait(char* id){
	t_semaforo* semaforo = get_semaforo_by_ID(semaforo_control_list, id);
	if(semaforo->sem_value > 0){
		semaforo->sem_value --;
		return true;
	}
	else{
		return false;
	}
}
//---------------------------------------------- </SEMÁFOROS>

//---------------------------------------------- <PCBs>

//Devuelve la cantidad de páginas requeridas en SWAP para salvar el código
int getProgramPagesCount(char* program){
	int pagesCount = 0;
	if(memoryPageSize != 0){
		pagesCount =  (strlen(program) / memoryPageSize) + stack_size;
		if(strlen(program) % memoryPageSize){
			pagesCount++;
		}
	}

	return pagesCount;
}

//Agrega pcb a la lista general de procesos
void add_pcb_to_general_list(PCB* pcb){
	list_add(General_Process_List, pcb);

	log_trace(nucleo_logger, "PLANIFICACION: Proceso %d agregado a la lista general de procesos", pcb->processId);
}

//Encola pcb en la cola general de listos
void set_pcb_READY(PCB* pcb){
	if(queue_is_empty(READY_Process_Queue)){
		free(READY_Process_Queue);
		READY_Process_Queue = queue_create();
	}
	queue_push(READY_Process_Queue, pcb);
	log_trace(nucleo_logger, "PLANIFICACION: Proceso %d READY", pcb->processId);
}

//Encola pcb en la cola general de Running
void set_pcb_RUNNING(PCB* pcb){
	list_add(RUNNING_Process_List, pcb);
	log_trace(nucleo_logger, "PLANIFICACION: Proceso %d RUNNING", pcb->processId);
}

//Encola pcb en la cola general de bloqueados
void set_pcb_BLOCKED(PCB* pcb){
	queue_push(BLOCKED_Process_Queue, pcb);
}

//Rutina de finalización de un programa
void set_pcb_EXIT(int processID){
	umc_notificarFinDePrograma(processID);		//Notifica fin de programa a UMC
	end_process(processID);						//Destruye las estructuras del proceso dentro del núcleo
	console_endProgram(processID);				//Notifica fin de programa a Consola
}

void change_status_RUNNING_to_READY(t_CPU* cpu){
	//sacar PCB de RUNNING
	PCB* readyPcb = remove_pcb_by_ID(RUNNING_Process_List, cpu->PID);
	if(readyPcb != 0) {
		log_trace(nucleo_logger, "PLANIFICACION: Proceso %d removido de RUNNING", readyPcb->processId);

		//TODO: Guti - TESTEAR
		//encolar en ready
		if(is_program_alive(readyPcb->processId)){
			set_pcb_READY(readyPcb);
		}
		else{
			log_warning(nucleo_logger, "Programa %d no seteado a READY por desconexión de consola", readyPcb->processId);
		}
	}

	liberarCpu(cpu->cpuSocket);
	log_trace(nucleo_logger, "CONTROL CPU: CPU %d libre", cpu->cpuSocket);
}

void change_status_RUNNING_to_BLOCKED(int PID, char* deviceID){
	//Sacar de la cola de running
	PCB* pcb = remove_pcb_by_ID(RUNNING_Process_List, deviceID);
	//Traer dispositivo
	t_IO_Device* device = get_device_by_id(deviceID);
	//Encolar en cola de bloqueados de dispositivo
	queue_push(BLOCKED_Process_Queue, device);
	log_trace(nucleo_logger, "ENTRADA/SALIDA: Programa %d esperando dispositivo %s", pcb->processId, device->ioId);
}

//Finalizar un proceso
void end_process(int PID){
	PCB* pcb = remove_pcb_by_ID(General_Process_List, PID);
	remove_pcb_by_ID(RUNNING_Process_List, PID);
	free_pcb(pcb);
}

void nucleo_updatePCB(PCB* newPCB){
	if(is_program_alive(newPCB->processId)){
		PCB* actualPCB = get_pcb_by_ID(General_Process_List, newPCB->processId);

		actualPCB->programCounter = newPCB->programCounter;
		actualPCB->memoryIndex = newPCB->memoryIndex;
		actualPCB->stackCount = newPCB->stackCount;

		//list_clean_and_destroy_elements(actualPCB->stack, free_stackContent);
		actualPCB->stack = newPCB->stack;
	}
	else{
		log_warning(nucleo_logger, "Programa %d no actualizado por desconexión de consola", newPCB->processId);
	}
	//list_add_all(actualPCB->stack, newPCB->stack);

	//Actualizar con datos que provienen del CPU
}

void initNewProgram(u_int32_t codeSize, char* programSourceCode, int consoleSocket){
	PCB* nuevoPCB;
	nuevoPCB = new_pcb(consoleSocket);
	int memoryPagesCount;
	memoryPagesCount = getProgramPagesCount(programSourceCode);
	create_program_PCB(nuevoPCB, programSourceCode, memoryPagesCount);
	//TODO:Sacar linea
	//add_pcb_to_general_list(nuevoPCB);

	//Envío solicitud de páginas a UMC
	umc_initProgram(memoryPagesCount, nuevoPCB, codeSize, programSourceCode);
}

//Toma el primer programa de la cola de READY y lo envía a cpu para su ejecución
void set_next_pcb_RUNNING(int cpu_id){
	t_CPU* cpu = get_CPU_by_socket(cpu_id);

	//TODO: Guti - TESTEAR
	PCB* pcbCandidate = queue_pop(READY_Process_Queue);
	while(!is_program_alive(pcbCandidate->processId)){
		free_pcb(pcbCandidate);
		pcbCandidate = queue_pop(READY_Process_Queue);
	}
	cpu->PID = pcbCandidate->processId;					//Asigno a la estructura CPU el id del proceso que va a ejecutar
	list_add(RUNNING_Process_List, pcbCandidate);
	cpu_sendPCB(pcbCandidate, cpu->cpuSocket);
}
//---------------------------------------------- </PCBs>

//---------------------------------------------- <VARIABLES>
//Setea el diccionario general de variables globales definidos por configuración, iniciándolas en cero
void set_vars_dictionary(){
	int i = 0;
	while(shared_values[i] != NULL){
		dictionary_put(vars_control_dictionary, shared_values[i], 0);
		i++;
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

void hiloDeLectura(t_config* config){
	while(1){
		quantum = config_get_int_value(config, "QUANTUM");
	}
}
//---------------------------------------------- </VARIABLES>

//---------------------------------------------- <PROGRAMA>
void init_threads_config(pthread_attr_t nucleo_attr){
	pthread_attr_init(&nucleo_attr);
	pthread_attr_setdetachstate(&nucleo_attr, PTHREAD_CREATE_DETACHED);
}

void init_cpu_communication_thread(pthread_attr_t nucleo_attr){
	pthread_t cpuCommunicationThread;
	pthread_create(&cpuCommunicationThread, &nucleo_attr, &cpu_comunication_program, NULL);
}

void init_console_communication_thread(pthread_attr_t nucleo_attr){
	pthread_t consoleCommunicationThread;
	pthread_create(&consoleCommunicationThread, &nucleo_attr, &console_comunication_program, NULL);
}

void init_planification_thread(pthread_attr_t nucleo_attr){
	pthread_t plannificationThread;		//Hilo de planificación
	pthread_create(&plannificationThread, &nucleo_attr, &plannificationProgram, NULL);
}
//---------------------------------------------- </PROGRAMA>

//---------------------------------------------- <CPU>
//Agrega un nuevo cpu a la lista general de control
void nucleo_nuevo_cpu(int cpu_socket){
	t_CPU* cpu = new_cpu(cpu_socket);
	add_new_cpu(cpu);
	log_trace(nucleo_logger, "COMUNICACION: nuevo cpu conectado  %d", cpu_socket);
}
//---------------------------------------------- </CPU>
