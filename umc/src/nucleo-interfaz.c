#include "nucleo-interfaz.h"
#include "swap-interfaz.h"

void recbirInitPrograma(int nucleo_socket){
	int32_t pid;
	int32_t cantPaginas;
	char* codFuente;

	message* pidMessage = receiveMessage(nucleo_socket);
	pid = convertToInt32(pidMessage->contenido);

	message* cantPaginasMessage = receiveMessage(nucleo_socket);
	cantPaginas = convertToInt32(cantPaginasMessage->contenido);

	message* codFuenteMessage = receiveMessage(nucleo_socket);
	codFuente = codFuenteMessage->contenido;

	log_trace(logger, "Pedido init program [pid: %d, cantPaginas: %d]", pid, cantPaginas);
	log_trace(logger, "[Cod fuente: %s]", codFuente);

	response* swapResponse = initProgramaSwap(&pid, &cantPaginas, codFuente);

	if(swapResponse->ok){
		response* umcInitPidResponse = initProgramaUMC(pid, cantPaginas);
		if(!umcInitPidResponse->ok){
			sendErrorMessage(nucleo_socket, HEADER_PAGINAS_NO_DISPONIBLES, umcInitPidResponse->codError);
			return;
		}
		sendMessage(nucleo_socket, HEADER_PAGINAS_DISPONIBLES, 0, 0);
		log_trace(logger, "Enviada respuesta OK! de init pid %d", pid);
	}

	if(!swapResponse->ok){
		sendErrorMessage(nucleo_socket, HEADER_PAGINAS_NO_DISPONIBLES, swapResponse->codError);
		log_warning(logger, "Enviada respuesta de fallo de init pid %d", pid);
	}

	free(codFuente);
}

void recibirFinalizarPrograma(int nucleo_socket){
	message* message = receiveMessage(nucleo_socket);
	int32_t pid;
	memcpy(&pid, message->contenido, sizeof(int32_t));

	log_trace(logger, "Pedido fin programa [pid: %d]", pid);

	response* result = finalizarPidDeUMC(pid);

	if(!result->ok){
		enviarFAIL(nucleo_socket, result->codError);
		return;
	}

	response* swapResponse = finalizarProgramaSwap(pid);
	log_trace(logger, "Pid %d finalizado", pid);
	enviarResponse(nucleo_socket, swapResponse);
	log_trace(logger, "Enviada respuesta OK! de fin pid %d", pid);
}
