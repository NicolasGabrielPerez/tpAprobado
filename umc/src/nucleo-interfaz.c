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

	response* swapResponse = initProgramaSwap(&pid, &cantPaginas, codFuente);

	if(swapResponse->ok){
		initProgramaUMC(pid, cantPaginas);
		sendMessage(nucleo_socket, HEADER_PAGINAS_DISPONIBLES, 0, 0);
	}

	if(!swapResponse->ok){
		sendErrorMessage(nucleo_socket, HEADER_PAGINAS_NO_DISPONIBLES, swapResponse->codError);
	}

	free(codFuente);
}

void recibirFinalizarPrograma(int nucleo_socket){
	message* message = receiveMessage(nucleo_socket);
	int32_t pid;
	memcpy(&pid, message->contenido, sizeof(int32_t));

	response* result = finalizarPidDeUMC(pid);

	if(!result->ok){
		enviarFAIL(nucleo_socket, result->codError);
		return;
	}

	response* swapResponse = finalizarProgramaSwap(&pid);
	enviarResponse(nucleo_socket, swapResponse);
}
