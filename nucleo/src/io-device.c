#include "io-device.h"

t_list* IO_Device_List;
t_queue* READY_Process_Queue;
char** io_ids;
char** io_sleep_times;

pthread_attr_t attr;

//Función que se cargará en el hilo de dispositivo
void ioDeviceProgram(t_IO_Device* device){
	while(1){
		if(!queue_is_empty(device->BlockedProcessesQueue)){
			attend_blocked_processes(device);
		}
	}
}

//Levanta los dispositivos externos desde configuración y arma una lista de t_IO_Device
void set_IO_devices_list(){
	int i = 0;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	while(io_ids[i] != NULL){
		t_IO_Device* device = malloc(sizeof(t_IO_Device));
		device->ioId = io_ids[i];
		device->sleepTime = io_sleep_times[i];
		device->BlockedProcessesQueue = queue_create();
		list_add(IO_Device_List, device);

		//TODO: Crear hilo por dispositivo
		pthread_t deviceThread;
		pthread_create(&deviceThread, &attr, &ioDeviceProgram, (void*) device);

		i++;
	}
	//TODO: debo liberar la memoria de los arrays de configuración????
}

//Encola pcb en la cola de bloqueados de device
void set_pcb_BLOCKED_by_device(PCB* pcb, t_IO_Device* device){
	queue_push(device->BlockedProcessesQueue, pcb);
}

void change_status_BLOCKED_to_READY(int PID){
//	TODO:Implementar
}

void execute_process_IO(int sleepTime){
	usleep(sleepTime);
}

//Ejecuta las IO encoladas del dispositivo y las encola en READY
void attend_blocked_processes(t_IO_Device* io_device){
	PCB* pcb;

	while(queue_size(io_device->BlockedProcessesQueue) > 0){
		pcb = queue_pop(io_device->BlockedProcessesQueue);
		execute_process_IO(io_device->sleepTime);
		queue_push(READY_Process_Queue, pcb);
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
		PCB* pcb;
		for(i = 0 ; i < list_size(IO_Device_List) ; i++){
			device = list_get(IO_Device_List, i);
			if (queue_size(device->BlockedProcessesQueue) > 0){
				attend_blocked_processes(device);
			}
		}
	}
}














//Redefinición de método para buscar dentro de una lista
static t_link_element* list_find_by_name(t_list *self, char* name, bool(*condition)(void*, int), int* index) {
	t_link_element *element = self->head;
	int position = 0;

	while (element != NULL && !condition(element->data, name)) {
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

