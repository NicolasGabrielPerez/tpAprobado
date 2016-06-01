#include "nucleo-interfaz.h"
#include "swap-interfaz.h"

void recbirInitPrograma(int nucleo_socket){
	int bytes_recibidos;
	int32_t pid;
	int32_t cantPaginas;
	int codFuente_size;
	char* codFuente;

	if ((bytes_recibidos = recv(nucleo_socket, &pid, sizeof(int32_t), 0)) == -1) {
		perror("recv");
		exit(1);
	}
	if ((bytes_recibidos = recv(nucleo_socket, &cantPaginas, sizeof(int32_t), 0)) == -1) {
		perror("recv");
		exit(1);
	}
	codFuente_size = cantPaginas*marco_size;
	codFuente = malloc(codFuente_size);
	if ((bytes_recibidos = recv(nucleo_socket, codFuente, codFuente_size, 0)) == -1) {
		perror("recv");
		exit(1);
	}

	response* swapResponse = initProgramaSwap(&pid, &cantPaginas, codFuente);
	enviarResponse(nucleo_socket, swapResponse);

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
