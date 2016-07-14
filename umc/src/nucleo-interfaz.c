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
		sendMessage(nucleo_socket, HEADER_PAGINAS_DISPONIBLES, 0, 0);
	}

	if(!swapResponse->ok){
		sendErrorMessage(nucleo_socket, HEADER_PAGINAS_NO_DISPONIBLES, swapResponse->codError);
	}

	free(codFuente);
}

void recibirFinalizarPrograma(int nucleo_socket){
	int32_t pid;
	char* respuesta;

	if (recv(nucleo_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	//TODO borrar de Memoria Principal

	response* swapResponse = finalizarProgramaSwap(&pid);
	enviarResponse(nucleo_socket, swapResponse);
}
