#include "umc-interfaz.h"
#include "nucleo-structs.h"
#include "consola-interfaz.h"


int socket_umc;

//Valida con la UMC si es posible almacenar el nuevo programa y lo almacena
int almacenamientoPosible(int paginas,PCB* nuevoPCB,char* ANSiSop){//TODO: hacer q envie el pid y verificar la respuesta es el mismo pid
	int bytes_recibidos;
	int32_t pid = nuevoPCB->processId;
	int32_t cantPaginas = paginas;
	int codFuente_size = sizeof(ANSiSop);
	char* codFuente = ANSiSop;

	//TODO: chequear como implementa UMC el pedido de espacio

	if ((bytes_recibidos = send(socket_umc, pid, sizeof(int32_t), 0)) == -1) {
		perror("recv");
		exit(1);
	}
	if ((bytes_recibidos = send(socket_umc, cantPaginas, sizeof(int32_t), 0)) == -1) {
		perror("recv");
		exit(1);
	}

	if ((bytes_recibidos = send(socket_umc, codFuente, codFuente_size, 0)) == -1) {
		perror("recv");
		exit(1);
	;
	}
	return bytes_recibidos;
	//TODO: Recibir respuesta de UMC (OK/No hay lugar) y retornarla
	//TODO: Hay que mandar stack?
}

void switchRecibirPorHEADER(){
	message* mensaje = receiveMessage(socket_umc);
	if(mensaje->header == HEADER_HANDSHAKE){
		handshake_con_UMC();
	}

	if(mensaje->header == HEADER_INIT_PROGRAMA){
		aceptarPrograma(mensaje);
	}

	//TODO: validar correctamente el header erróneo
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

//Envía el PID del programa a finalizar
void notificarFinDePrograma(int processID){
	char* stringPID = string_itoa(processID);		//Convierte el ID a string
	sendMessage(socket_umc, HEADER_FIN_PROGRAMA, sizeof(stringPID), stringPID);
}

void conectarConUMC(t_config* config){
	 char* puerto_umc = config_get_string_value(config, "PUERTO_UMC"); //puerto de UMC
	 socket_umc = crear_socket_cliente("utnso40", puerto_umc);
}
	 //TODO: crear función para enviar header
void handshake_con_UMC(){

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


