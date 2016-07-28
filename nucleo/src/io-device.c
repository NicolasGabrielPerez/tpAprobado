#include "io-device.h"

t_list* IO_Device_List;
t_list* RUNNING_Process_List;
t_queue* READY_Process_Queue;
char** io_ids;
char** io_sleep_times;

int io_thread_sleep;

pthread_attr_t nucleo_attr;

//Función que se cargará en el hilo de dispositivo
void ioDeviceProgram(t_IO_Device* device){
	printf("Iniciado dispositivo %s [Sleep time %d]", device->ioId, device->sleepTime);
	while(1){
		if(!queue_is_empty(device->BlockedProcessesQueue)){
			attend_blocked_processes(device);
		}
		//sleep(io_thread_sleep);
	}
}

//Levanta los dispositivos externos desde configuración y arma una lista de t_IO_Device
void set_IO_devices_list(){
	int i = 0;

	pthread_attr_init(&nucleo_attr);
	pthread_attr_setdetachstate(&nucleo_attr, PTHREAD_CREATE_DETACHED);

	while(io_ids[i] != NULL){
		t_IO_Device* device = malloc(sizeof(t_IO_Device));
		device->ioId = io_ids[i];
		device->sleepTime = atoi(io_sleep_times[i]);
		device->BlockedProcessesQueue = queue_create();
		list_add(IO_Device_List, device);

		pthread_t deviceThread;
		pthread_create(&deviceThread, &nucleo_attr, &ioDeviceProgram, (void*) device);

		i++;
	}
	//TODO: debo liberar la memoria de los arrays de configuración????
}

//Instrucción privilegiada
//Encola pcb en la cola de bloqueados de device
void set_pcb_BLOCKED_by_device(PCB* pcb, t_IO_Device* device){
	queue_push(device->BlockedProcessesQueue, pcb);
}

void execute_process_IO(int sleepTime){
	usleep(sleepTime * 1000);
}

//Ejecuta TODAS las IO encoladas del dispositivo y las encola en READY
void attend_blocked_processes(t_IO_Device* io_device){
	PCB* pcb;

	while(queue_size(io_device->BlockedProcessesQueue) > 0){
		pcb = queue_pop(io_device->BlockedProcessesQueue);
		execute_process_IO(io_device->sleepTime * pcb->sleepUnits);
		set_pcb_READY(pcb);
	}
}

t_IO_Device* get_device_by_id(char* ioID){
	t_IO_Device* device = device_list_find_element(IO_Device_List, ioID, find_device);
	return device;
}

int exists_any_blocked_process(){
	int i;
	t_IO_Device* device;
	for(i = 0 ; i < list_size(IO_Device_List) ; i++){
		device = list_get(IO_Device_List, i);
		if (queue_size(device->BlockedProcessesQueue) > 0){
			return 1;
		}
	}
	return 0;
}

void execute_all_processes_IO(){
	if(exists_any_blocked_process()){
		int i;
		t_IO_Device* device;
		for(i = 0 ; i < list_size(IO_Device_List) ; i++){
			device = list_get(IO_Device_List, i);
			if (queue_size(device->BlockedProcessesQueue) > 0){
				attend_blocked_processes(device);
			}
		}
	}
}

void process_call_io(char* deviceName, int32_t PID, int32_t time){
	t_IO_Device* device = get_device_by_id(deviceName);

	PCB* pcb = remove_pcb_by_ID(RUNNING_Process_List, PID); //Saco PCB de lista de READY (desactualizado)
	pcb->sleepUnits = time;
	set_pcb_BLOCKED_by_device(pcb, device);					//Encolar en la cola de bloqueados de device
}












//Redefinición de método para buscar dentro de una lista
static t_link_element* list_find_by_name(t_list *self, char* name, bool(*condition)(void*, int), int* index) {
	t_link_element *element = self->head;
	int position = 0;

	while (element != NULL) {
		if(!condition(element->data, name)){
			return element;
		}
		element = element->next;
		position++;
	}

	if (index != NULL) {
		*index = position;
	}

	return element;
}

//Redefinición de método. Devuelve un t_IO_Device según ioID
t_IO_Device* device_list_find_element(t_list *self, char* ioID, bool(*condition)(void*, int)) {
	t_link_element *element = list_find_by_name(self, ioID,condition, NULL);
	return element != NULL ? element->data : NULL;
}

int find_device(t_IO_Device* device, char* ioID){
	return strcmp(device->ioId, ioID);
}

