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
		Handshake_con_UMC();
	}


	if(mensaje->header == HEADER_INIT_PROGRAMA){
		aceptarPrograma(mensaje);
	}
	if(mensaje->header == HEADER_FIN_PROGRAMA){
	finalizarProgramaenUMC(mensaje);
	}
	perror("header invalido");
	enviarFAIL(socket_umc, HEADER_INVALIDO);


}

void finalizarProgramaenUMC(message* mensaje){
	PCB* nuevoPCB = deserialize_pcb( mensaje->contenido);
	endOfProgram(nuevoPCB->processId);
	int size = sizeof(nuevoPCB->processId);
	sendMessage(socket_umc, HEADER_FIN_PROGRAMA, size, nuevoPCB->processId);
	free_pcb(nuevoPCB);

}

void aceptarPrograma(message* mensaje){
	PCB* nuevoPCB = deserialize_pcb( mensaje->contenido);

	add_pcb_to_general_list(nuevoPCB);

}


void notificarFinDePrograma(){
	sendMessage(socket_umc, HEADER_FIN_PROGRAMA, 0, "");
}

vfinalizarProgrmaEnUMC(int processId){


}
void conectarConUMC(t_config* config){
	 char* puerto_umc = config_get_string_value(config, "PUERTO_UMC"); //puerto de UMC
	 socket_umc = crear_socket_cliente("utnso40", puerto_umc);

	 //TODO: crear función para enviar header

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
