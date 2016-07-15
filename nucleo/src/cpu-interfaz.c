#include "cpu-interfaz.h"
#include "umc-interfaz.h"


int cpu_listener;
fd_set cpu_sockets_set;
int fd_cpu_max;

// SEND
void sendPCB(int socket ,PCB* unPCB){
	Buffer *buffer = malloc(sizeof(Buffer));
	buffer = new_buffer();
	char* pcbzerial = serialize_pcb(unPCB ,buffer);
	int size = sizeof(pcbzerial);
	buffer_free( buffer);
	sendMessage(socket, HEADER_ENVIAR_PCB,size , pcbzerial);
}

void notificar_a_cpu_su_muerte(int socket){

	sendMessage(socket, SIGUSR1, 0 , "");
}

void handleCpuRequests(int socket){

	message* mensaje;
	mensaje = receiveMessage(socket);

	if(mensaje->header == HEADER_HANDSHAKE){
		handShakeWithCPU(socket);
	}

	if(mensaje->header == HEADER_NOTIFICAR_IO){
		t_CPU* cpu = get_CPU_by_socket(socket);						//Obtengo estructura CPU
		process_call_io(mensaje->contenido, cpu->PID);

		//Pedir PCB
		sendMessage(socket, HEADER_ENVIAR_PCB, 0, 0);
		//Al pedir PCB, CPU debe enviarlo
	}

	//Recepción de estructura PCB desde CPU => actualiza PCB
	if(mensaje->header == HEADER_ENVIAR_PCB){
		PCB* pcb = deserialize_pcb(mensaje->contenido);
		nucleo_updatePCB(pcb);
	}

	if(mensaje->header == HEADER_NOTIFICAR_FIN_QUANTUM){
		nucleo_notificarFinDeQuantum(mensaje);
	}
	if(mensaje->header == HEADER_FIN_PROGRAMA){
		t_CPU* cpu = get_CPU_by_socket(socket);			//Obtengo estructura CPU
		finalizarFelizmenteTodo(cpu->PID);				//Me encargo de notificar y destruir estructuras correspondientes
	}
	if(mensaje->header == HEADER_NOTIFICAR_FIN_RAFAGA){
		//nucleo_notificarFinDeRafaga(mensaje);
		t_CPU* cpu = get_CPU_by_socket(socket);			//Obtengo estructura CPU
		change_status_RUNNING_to_READY(cpu);			//Mover PCB a cola de READY
		sendMessage(socket, HEADER_ENVIAR_PCB, 0, 0);	//Pedir PCB
	}
	if(mensaje->header == HEADER_NOTIFICAR_WAIT){
		t_CPU* cpu = get_CPU_by_socket(socket);
		nucleo_wait(mensaje, cpu);
	}
	if(mensaje->header == HEADER_NOTIFICAR_SIGNAL){
		nucleo_signal(mensaje);
	}
	if(mensaje->header == HEADER_IMPRIMIR){
		t_CPU* cpu = get_CPU_by_socket(socket);
		nucleo_imprimir(mensaje, cpu);
	}
	if(mensaje->header == HEADER_IMPRIMIR_TEXTO){
		t_CPU* cpu = get_CPU_by_socket(socket);
		nucleo_imprimir_texto(mensaje, cpu);
	}
	//perror("header invalido");
	//enviarFAIL(socket, HEADER_INVALIDO);

	if(mensaje->header == HEADER_SETEAR_VARIABLE){
		t_globalVar* var = deserialize_globalVar(mensaje->contenido);
		nucleo_setear_variable(var);
	}

	if(mensaje->header == HEADER_OBTENER_VARIABLE){
		t_CPU* cpu = get_CPU_by_socket(socket);
		nucleo_obtener_variable(mensaje, cpu);
	}
}

PCB* nucleo_obtener_pcb(message* programBlock){

	char* PCBSerialized = malloc(programBlock->contenidoSize);
	PCBSerialized = programBlock->contenido;

	PCB* Deserializado = deserialize_pcb(PCBSerialized);
	return Deserializado;
}

void nucleo_notificarFinDeQuantum(message* mensaje){
	//TODO: loguear evento
}

void nucleo_notificarFinDeRafaga(message* mensaje){}

void nucleo_notificarFinDePrograma(message* mensaje){//FINALIZADA
	//int32_t PID
	//finalizarFelizmenteTodo();
}

void nucleo_wait(message* mensaje, t_CPU* cpu){
	t_semaforo* semaforo = get_semaforo_by_ID(semaforo_control_list, mensaje->contenido);
	if(wait(semaforo->sem_id)){
		sendMessage(cpu->cpuSocket, HEADER_WAIT_CONTINUAR, 0, 0);
	}
	else{
		sendMessage(cpu->cpuSocket, HEADER_ENVIAR_PCB, 0, 0);
		//Bloquear proceso por semáforo
		PCB* pcb = get_pcb_by_ID(General_Process_List, cpu->PID);
		queue_blocked_process_to_semaforo(semaforo->sem_id, pcb);
	}
}

void nucleo_signal(message* mensaje){
	signal(mensaje->contenido);
}

void nucleo_imprimir_texto(message* mensaje, t_CPU* cpu){
	char* result = malloc(mensaje->contenidoSize);
	result = mensaje->contenido;
	sendResults(cpu->PID, result);
	free(result);
}

void nucleo_imprimir(message* mensaje, t_CPU* cpu){
	char* result = malloc(sizeof("mensaje harcodeado de nucleo imprimir")+1);
	result = "mensaje harcodeado de nucleo imprimir";
	sendResults(cpu->PID, result);
}

void nucleo_obtener_variable(message* mensaje, t_CPU* cpu){
	int value = get_var_value(mensaje->contenido);
	sendMessageInt(cpu->cpuSocket, HEADER_OBTENER_VARIABLE, value);
}

void nucleo_setear_variable(t_globalVar* var){
	set_var_value(var->varName, var->value);
}

//No se precisa
void sendInstruction(int socket,char* instruccion){
	int contenidoSize = sizeof(instruccion);

	if (enviarOKConContenido(socket,contenidoSize,instruccion)<0){
		perror ("send instruccion");
		exit(1);
	}

}

void initCPUListener(t_config* config){
	char* puerto_cpu = config_get_string_value(config, "PUERTO_CPU");
	cpu_listener = crear_puerto_escucha(puerto_cpu);
	FD_ZERO(&cpu_sockets_set);    // clear the master and temp sets
	FD_SET(cpu_listener, &cpu_sockets_set);
	fd_cpu_max = cpu_listener;
}

void actualizarFdCPUMax(int socket){
	if(socket>fd_cpu_max) fd_cpu_max = socket;
}

void handShakeWithCPU(int cpu_socket){
	sendMessage(cpu_socket, HEADER_HANDSHAKE, 0, 0);
}

void finPrograma(int cpu_socket){
	sendMessage(cpu_socket, HEADER_FIN_PROGRAMA, 0, "");
}

void handleCPURequest(int cpu_socket){
	char* header = malloc(HEADER_SIZE);
	if (recv((int)socket, header, HEADER_SIZE, 0) == -1) {
		perror("recv");
		exit(1);
	}
	int32_t iHeader = convertToInt32(header);
	if(iHeader == HEADER_HANDSHAKE){
		handShakeWithCPU(cpu_socket);
		return;
	}
	if(iHeader == FIN_PROGRAMA){
		finPrograma(cpu_socket);
		return;
	}

	enviarFAIL(cpu_socket, HEADER_INVALIDO);
}

void manejarSocketChanges(int socket, fd_set* read_set){
	if (FD_ISSET(socket, read_set)) {
		if (socket == cpu_listener) { //nuevo cpu
			int new_fd = aceptarNuevaConexion(cpu_listener);
			actualizarFdCPUMax(new_fd);
		} else{
			//handleCPURequest(socket);
			handleCpuRequests(socket);
		}
	};
}

void manejarConexionesCPUs(){
	fd_set read_fds; //set auxiliar
	read_fds = cpu_sockets_set;
	if (select(fd_cpu_max+1, &read_fds, NULL, NULL, NULL) == -1) {
		perror("select");
		exit(4);
	}
	int i;
	for(i = 0; i <= fd_cpu_max; i++) {
		manejarSocketChanges(i, &read_fds);
	};
}

void cpu_comunication_program(){
	while(1){
		manejarConexionesCPUs();
	}
}
