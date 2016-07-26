#include "umc-interfaz.h"
#include "nucleo-structs.h"
#include "consola-interfaz.h"


int socket_umc;

//---------------------------------------------- <SEND>
//Valida con la UMC si es posible almacenar el nuevo programa y lo almacena
void umc_initProgram(u_int32_t pagesCount, PCB* pcb, u_int32_t programSize, char* program){
	log_trace(nucleo_logger, "COMUNICACIÓN: UMC - Enviando nuevo programa ID:%d", pcb->processId);
	sendMessage(socket_umc, HEADER_INIT_PROGRAMA, 0, 0);
	sendMessageInt(socket_umc, HEADER_SOLICITAR_PAGINAS, pcb->processId);
	sendMessageInt(socket_umc, HEADER_SOLICITAR_PAGINAS, pagesCount);
	sendMessage(socket_umc, HEADER_SOLICITAR_PAGINAS, programSize, program);

	message* message = receiveMessage(socket_umc);
	if(message->header == HEADER_PAGINAS_DISPONIBLES){
		log_trace(nucleo_logger, "UMC: Páginas disponibles, Programa aceptado ID: %d", pcb->processId);
		//Notificar programa aceptado
		sendMessage(pcb->processId, HEADER_PAGINAS_DISPONIBLES, 0, 0);
		//Agregar PCB a la lista general
		add_pcb_to_general_list(pcb);
		//Mover PCB a cola de READY
		set_pcb_READY(pcb);
	}

	if(message->header == HEADER_PAGINAS_NO_DISPONIBLES){
		log_trace(nucleo_logger, "UMC: Páginas no disponibles, Programa rechazado ID: %d", pcb->processId);
		printf("Código de error: %d \n", message->codError);
		//Notificar consola programa rechazado
		sendMessage(pcb->processId, HEADER_PAGINAS_NO_DISPONIBLES, 0, 0);
		//Matar PCB
		free_pcb(pcb);
	}

	if(message->contenidoSize > 0) free(message->contenido);
	free(message);
}

//Envía orden de finalización de programa a UMC
void umc_endProgram(u_int32_t PID){
	log_trace(nucleo_logger, "COMUNICACIÓN: UMC - Finalizar programa ID: %d", PID);
	sendMessageInt(socket_umc, HEADER_FIN_PROGRAMA, PID);
	sendMessageInt(socket_umc, HEADER_FIN_PROGRAMA, PID);
}

//Envía el PID del programa a finalizar
void umc_notificarFinDePrograma(int processID){
	log_trace(nucleo_logger, "COMUNICACIÓN: UMC - Fin de programa ID: %d", processID);
	sendMessageInt(socket_umc, HEADER_FIN_PROGRAMA, processID);
	sendMessageInt(socket_umc, HEADER_FIN_PROGRAMA, processID); //Se tiene que enviar dos veces, NO esta repetido.
}

void handshake_con_UMC(){
	log_trace(nucleo_logger, "COMUNICACIÓN: UMC - Handshake realizado");
	sendMessageInt(socket_umc,HEADER_HANDSHAKE, TIPO_NUCLEO);
	//Recibir tamaño de página
	message* pageSizeMessage = receiveMessage(socket_umc);

	memoryPageSize = convertToInt32(pageSizeMessage->contenido);

	free(pageSizeMessage->contenido);
	free(pageSizeMessage);
}
//---------------------------------------------- </SEND>
void aceptarPrograma(message* mensaje){
	PCB* nuevoPCB = deserialize_pcb( mensaje->contenido);
	add_pcb_to_general_list(nuevoPCB);
}

void conectarConUMC(t_config* config){
	char* puerto_umc = config_get_string_value(config, "PUERTO_UMC"); //puerto de UMC
	socket_umc = crear_socket_cliente(umc_ip, puerto_umc);

	handshake_con_UMC();

	log_trace(nucleo_logger, "COMUNICACIÓN: Realizada conexión con UMC");
}



