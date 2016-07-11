#include "umc-interfaz.h"
#include "nucleo-structs.h"
#include "consola-interfaz.h"


int socket_umc;

void almacenamientoPosible(int paginas,PCB* nuevoPCB,char* ANSiSop){//TODO: hacer q envie el pid y verificar la respuesta es el mismo pid
	int bytes_recibidos;
	int32_t pid = nuevoPCB->processId;
	int32_t cantPaginas = paginas;
	int codFuente_size = sizeof(ANSiSop);
	char* codFuente = ANSiSop;

	if ((bytes_recibidos = send(socket_umc, pid, sizeof(int32_t), 0)) == -1) {
		perror("recv");
		exit(1);
	}
	if ((bytes_recibidos = send(socket_umc, &cantPaginas, sizeof(int32_t), 0)) == -1) {
		perror("recv");
		exit(1);
	}


	if ((bytes_recibidos = send(socket_umc, codFuente, codFuente_size, 0)) == -1) {
		perror("recv");
		exit(1);
	}
}


void swichRecivirPorHEADER(){
	message* mensaje = receiveMessage(socket_umc);
	if(mensaje->header == HEADER_HANDSHAKE){

	}

	if(mensaje->header == HEADER_ENVIAR_PCB){
		 hacerProtocoloPCB(mensaje);

	}
	if(mensaje->header == HEADER_PAGINAS_DISPONIBLES){
		aceptarPrograma(mensaje);
	}
	if(mensaje->header == HEADER_PAGINAS_NO_DISPONIBLES){
	rechazarPrograma(mensaje);
	}
	perror("header invalido");
	enviarFAIL(socket_umc, HEADER_INVALIDO);


}

void rechazarPrograma(message* mensaje){
	PCB* nuevoPCB = deserialize_pcb( mensaje->contenido);
	endOfProgram(nuevoPCB->processId);
	free_pcb(nuevoPCB);

}

void aceptarPrograma(message* mensaje){
	PCB* nuevoPCB = deserialize_pcb( mensaje->contenido);
	add_pcb_to_general_list(nuevoPCB);

}

void hacerProtocoloPCB(message* mensaje){


}
//TODO: dessarollar funciones al recivir un pcb de una umc
PCB* recivirPCB(message pcbSerial){

	PCB* unpcb = deserialize_pcb(pcbSerial->contenido);

return unpcb;
}


// SEND
void sendoPCB(PCB* unPCB){
	Buffer *buffer = malloc(sizeof(Buffer));
	buffer = new_buffer();
	char* pcbzerial = serialize_pcb(unPCB ,buffer);
	int size = sizeof(pcbzerial);
	buffer_free( buffer);
	sendMessage(socket_umc, HEADER_ENVIAR_PCB,size , pcbzerial);
	}

void notificarFinDePrograma(){
	sendMessage(socket_umc, HEADER_FIN_PROGRAMA, 0, "");
}


void conectarConUMC(t_config* config){
	 char* puerto_umc = config_get_string_value(config, "PUERTO_UMC"); //puerto de UMC
	 socket_umc = crear_socket_cliente("utnso40", puerto_umc);

	 //TODO: crear función para enviar header
	 //Handshake con UMC
	 char* content = string_itoa(HEADER_HANDSHAKE);
	 int32_t size = sizeof(int32_t);
	 enviarOKConContenido(socket_umc, size, content);

	 //Tipo de handshake
	 content = string_itoa(TIPO_NUCLEO);
	 size = sizeof(int32_t);
	 enviarOKConContenido(socket_umc, size, content);

	 //Recibir tamaño de página
	 response* UmcResponse = recibirResponse(socket_umc);
	 if(UmcResponse->ok){
		 memoryPageSize = convertToInt32(UmcResponse->contenido);
	 }
	 else{
		 puts("Tamaño de página no recibido");

	 }
}
