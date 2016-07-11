#include "cpu-interfaz.h"
#include <sockets/communication.h>
#include <sockets/serialization.h>

int cpu_listener;
fd_set cpu_sockets_set;
int fd_cpu_max;


// SEND
void sendoPCB(int socket ,PCB* unPCB){
	Buffer *buffer = malloc(sizeof(Buffer));
	buffer = new_buffer();
	char* pcbzerial = serialize_pcb(unPCB ,buffer);
	int size = sizeof(pcbzerial);
	buffer_free( buffer);
	sendMessage(socket, HEADER_ENVIAR_PCB,size , pcbzerial);
	}

void swichCPU_HEADER(int socket){

	message* mensaje;
	mensaje = receiveMessage(socket);


		if(mensaje->header == HEADER_HANDSHAKE){
			makeHandshakeWithCPU(socket);
		}
		if(mensaje->header == HEADER_ENVIAR_PCB){
			protocoloConPCBllegado(mensaje);
		}

		if(mensaje->header == HEADER_NOTIFICAR_IO){
			nucleo_notificarIO(mensaje);
		}

		if(mensaje->header == HEADER_NOTIFICAR_FIN_QUANTUM){
			nucleo_notificarFinDeQuantum(mensaje);
		}
		if(mensaje->header == HEADER_FIN_PROGRAMA){
			nucleo_notificarFinDePrograma(mensaje);
		}
		if(mensaje->header == HEADER_NOTIFICAR_FIN_RAFAGA){
		nucleo_notificarFinDeRafaga(mensaje);
		}
		if(mensaje->header == HEADER_NOTIFICAR_WAIT){
			nucleo_wait(mensaje);
		}
		if(mensaje->header == HEADER_NOTIFICAR_SIGNAL){
			nucleo_signal(mensaje);
		}
		if(mensaje->header == HEADER_IMPRIMIR){
			nucleo_imprimir(mensaje);
		}
		if(mensaje->header == HEADER_IMPRIMIR_TEXTO){
			nucleo_imprimir_texto(mensaje);
		}
		perror("header invalido");
		enviarFAIL(socket, HEADER_INVALIDO);
}

//TODO terminar protocolo
void protocoloConPCBllegado(message* mensaje){
	PCB* unpcb = nucleo_recibir_pcb(mensaje);
}

PCB* nucleo_recibir_pcb(message* programBlock){

	char* PCBSerialized = malloc(programBlock->contenidoSize);
	PCBSerialized = programBlock->contenido;

	PCB* Deserializado = deserialize_pcb(PCBSerialized);
	return Deserializado;
}
//TODO: Implementar
void nucleo_notificarIO(message* mensaje){

}


void nucleo_notificarFinDeQuantum(message* mensaje){
	message* programBlock;

	//programBlock = receiveMessage(socket); //Se agregó el igual a la asignación
}



char* nucleo_notificarFinDeRafaga(message* mensaje){
	char* v;	//Agrego variable para evitar error de sintaxis
	return v;
}
void nucleo_notificarFinDePrograma(message* mensaje){}
void nucleo_wait(message* mensaje){}
void nucleo_signal(message* mensaje){}
void nucleo_imprimir(message* mensaje){}
void nucleo_imprimir_texto(message* mensaje){}

//duda sobre estas dos
t_valor_variable nucleo_variable_compartida_obtener(t_nombre_compartida variable){
	t_valor_variable v;	//Agrego variable para evitar error de sintaxis
	return v;
}
void nucleo_variable_compartida_asignar(t_nombre_compartida variable, t_valor_variable valor){}


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

void makeHandshakeWithCPU(int cpu_socket){
	enviarOKConContenido(cpu_socket, 11, "Soy Nucleo");
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
		makeHandshakeWithCPU(cpu_socket);
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
		   handleCPURequest(socket);
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
