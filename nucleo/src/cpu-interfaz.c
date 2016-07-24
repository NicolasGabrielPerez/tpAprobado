#include "cpu-interfaz.h"
#include "umc-interfaz.h"
#include "nucleo-structs.h"

u_int32_t quantum;
int cpu_listener;
fd_set cpu_sockets_set;
int fd_cpu_max;

int test_mode;

void handleCpuRequests(int socket){

	message* mensaje;
	mensaje = receiveMessage(socket);

	if(mensaje->codError == SOCKET_DESCONECTADO){
		log_warning(nucleo_logger, "Socket de CPU %d desconectado\n", socket);
		//TODO: Implementar apagado de CPU
		//Sacar estructura de control de la lista general de cpu
		//Mover PCB de lista de RUNNING a READY
	}

	if(!test_mode){
		if(mensaje->header == HEADER_HANDSHAKE){
			handShakeWithCPU(socket);
			log_trace(nucleo_logger, "COMUNICACIÓN: Nuevo CPU conectado en %d", socket);
		}

		if(mensaje->header == HEADER_NOTIFICAR_IO){
			log_trace(nucleo_logger, "COMUNICACIÓN: Solicitud E/S desde CPU %d", socket);
			t_CPU* cpu = get_CPU_by_socket(socket);						//Obtengo estructura CPU
			process_call_io(mensaje->contenido, cpu->PID);

			//Pedir PCB
			//sendMessage(socket, HEADER_ENVIAR_PCB, 0, 0);
			//Al pedir PCB, CPU debe enviarlo
		}

		//Recepción de estructura PCB desde CPU => actualiza PCB
		if(mensaje->header == HEADER_ENVIAR_PCB){
			log_trace(nucleo_logger, "COMUNICACIÓN: Recibido PCB desde CPU %d", socket);
			PCB* pcb = deserialize_pcb(mensaje->contenido);
			nucleo_updatePCB(pcb);
		}

		if(mensaje->header == HEADER_NOTIFICAR_FIN_QUANTUM){
			log_trace(nucleo_logger, "COMUNICACIÓN: Recibido fin de quantum desde CPU %d", socket);
			t_CPU* cpu = get_CPU_by_socket(socket);
			nucleo_notificarFinDeQuantum(mensaje, cpu);
		}
		if(mensaje->header == HEADER_FIN_PROGRAMA){
			log_trace(nucleo_logger, "COMUNICACIÓN: Recibido fin de programa desde CPU %d", socket);
			t_CPU* cpu = get_CPU_by_socket(socket);			//Obtengo estructura CPU
			set_pcb_EXIT(cpu->PID);				//Me encargo de notificar y destruir estructuras correspondientes
		}
		if(mensaje->header == HEADER_NOTIFICAR_FIN_RAFAGA){
			log_trace(nucleo_logger, "COMUNICACIÓN: Recibido fin de ráfaga desde CPU %d", socket);
			nucleo_notificarFinDeRafaga(mensaje, socket);
		}
		if(mensaje->header == HEADER_NOTIFICAR_WAIT){
			log_trace(nucleo_logger, "COMUNICACIÓN: WAIT() desde CPU %d", socket);
			t_CPU* cpu = get_CPU_by_socket(socket);
			nucleo_wait(mensaje, cpu);
		}
		if(mensaje->header == HEADER_NOTIFICAR_SIGNAL){
			log_trace(nucleo_logger, "COMUNICACIÓN: SIGNAL() desde CPU %d", socket);
			nucleo_signal(mensaje);
		}
		/*if(mensaje->header == HEADER_IMPRIMIR){
		t_CPU* cpu = get_CPU_by_socket(socket);
		nucleo_imprimir(mensaje, cpu);
	}*/
		if(mensaje->header == HEADER_IMPRIMIR_TEXTO){
			log_trace(nucleo_logger, "COMUNICACIÓN: Imprimir texto desde CPU %d", socket);
			t_CPU* cpu = get_CPU_by_socket(socket);
			nucleo_imprimir_texto(mensaje, cpu);
		}
		//perror("header invalido");
		//enviarFAIL(socket, HEADER_INVALIDO);

		if(mensaje->header == HEADER_SETEAR_VARIABLE){
			log_trace(nucleo_logger, "COMUNICACIÓN: Setear variable desde CPU %d", socket);
			t_globalVar* var = deserialize_globalVar(mensaje->contenido);
			nucleo_setear_variable(var);
		}

		if(mensaje->header == HEADER_OBTENER_VARIABLE){
			log_trace(nucleo_logger, "COMUNICACIÓN: Obtener variable desde CPU %d", socket);
			t_CPU* cpu = get_CPU_by_socket(socket);
			nucleo_obtener_variable(mensaje, cpu);
		}
	}
	else{	//Modo Test
		log_trace(nucleo_logger, "COMMUNICATION TEST: ID Mensaje %d , Contenido: %s", mensaje->header, mensaje->contenido);
	}
}

//---------------------------------------------- <SEND>
void handShakeWithCPU(int cpu_socket){
	sendMessage(cpu_socket, HEADER_HANDSHAKE, 0, 0);
}

void cpu_sendPCB(PCB* pcb, int cpu_socket){
	Buffer* buffer = new_buffer();
	char* serialized_pcb = serialize_pcb(pcb, buffer);
	int size = strlen(serialized_pcb);

	sendMessage(cpu_socket, HEADER_ENVIAR_PCB, size, serialized_pcb);
	cpu_sendQuantum(cpu_socket);
}

void cpu_sendQuantum(int cpu_socket){
	sendMessageInt(cpu_socket, HEADER_ENVIAR_QUANTUM, quantum);
}
//---------------------------------------------- </SEND>

//---------------------------------------------- <RECEIVE>
PCB* nucleo_obtener_pcb(message* programBlock){

	char* PCBSerialized = malloc(programBlock->contenidoSize);
	PCBSerialized = programBlock->contenido;

	PCB* Deserializado = deserialize_pcb(PCBSerialized);
	return Deserializado;
}

void nucleo_notificarFinDeQuantum(message* mensaje, t_CPU* cpu){
	log_trace(nucleo_logger, "PROGRAMA %d: Ejecución de quantum %s ", cpu->PID, mensaje->contenido);
}

void nucleo_notificarFinDeRafaga(message* mensaje, int socket){
	t_CPU* cpu = get_CPU_by_socket(socket);			//Obtengo estructura CPU
	change_status_RUNNING_to_READY(cpu);			//Mover PCB a cola de READY
}

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
		//Bloquear proceso por semáforo
		PCB* pcb = get_pcb_by_ID(General_Process_List, cpu->PID);
		queue_blocked_process_to_semaforo(semaforo->sem_id, pcb);
		//Setear cpu libre
		cpu->PID = -1;
	}
}

void nucleo_signal(message* mensaje){
	signal(mensaje->contenido);
}

void nucleo_imprimir_texto(message* mensaje, t_CPU* cpu){
	console_sendResults(cpu->PID, mensaje);
}

void nucleo_imprimir(message* mensaje, t_CPU* cpu){
	char* result = malloc(sizeof("mensaje harcodeado de nucleo imprimir")+1);
	result = "mensaje harcodeado de nucleo imprimir";
	console_sendResults(cpu->PID, result);
}

void nucleo_obtener_variable(message* mensaje, t_CPU* cpu){
	int value = get_var_value(mensaje->contenido);
	sendMessageInt(cpu->cpuSocket, HEADER_OBTENER_VARIABLE, value);
}

void nucleo_setear_variable(t_globalVar* var){
	set_var_value(var->varName, var->value);
}
//---------------------------------------------- </RECEIVE>

//---------------------------------------------- <COMMUNICATION>
void com_initCPUListener(t_config* config){
	char* puerto_cpu = config_get_string_value(config, "PUERTO_CPU");
	cpu_listener = crear_puerto_escucha(puerto_cpu);
	FD_ZERO(&cpu_sockets_set);    // clear the master and temp sets
	FD_SET(cpu_listener, &cpu_sockets_set);
	fd_cpu_max = cpu_listener;
}

void com_actualizarFdCPUMax(int socket){
	if(socket>fd_cpu_max) fd_cpu_max = socket;
}

void com_cpuManejarSocketChanges(int socket, fd_set* read_set){
	if (FD_ISSET(socket, read_set)) {

		if (socket == cpu_listener) { //nuevo cpu
			int new_fd = aceptarNuevaConexion(cpu_listener);
			com_actualizarFdCPUMax(new_fd);

			handleCpuRequests(new_fd);

			//Agrego un nuevo cpu a la lista de control
			nucleo_nuevo_cpu(new_fd);
			FD_SET(new_fd, read_set);
		}
		else{
			handleCpuRequests(socket);
		}
	}
}

void com_manejarConexionesCPUs(){
	fd_set read_fds; //set auxiliar
		read_fds = cpu_sockets_set;

		while(1){

			if (select(fd_cpu_max+1, &read_fds, NULL, NULL, NULL) == -1) {
				perror("select");
				exit(4);
			}
			int i;
			for(i = 0; i <= fd_cpu_max; i++) {
				com_cpuManejarSocketChanges(i, &read_fds);
			};
		}
}

void* cpu_comunication_program(){
	fd_set read_fds; //set auxiliar
	read_fds = cpu_sockets_set;

	while(1){

		if (select(fd_cpu_max+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}
		int i;
		for(i = 0; i <= fd_cpu_max; i++) {
			com_cpuManejarSocketChanges(i, &read_fds);
		};
	}
}
//---------------------------------------------- </COMMUNICATION>
