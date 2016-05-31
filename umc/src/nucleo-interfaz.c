#include "nucleo-interfaz.h"
#include "swap-interfaz.h"

char* recbirInitPrograma(int nucleo_socket){
	int bytes_recibidos;
	int32_t pid;
	int32_t cantPaginas;
	int codFuente_size;
	char* codFuente;
	char* respuesta;

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

	respuesta = initProgramaSwap(&pid, &cantPaginas, codFuente);

	int32_t respuestaInt;
	memcpy(&respuestaInt, respuesta, RESPUESTA_SIZE);

	if(respuestaInt == RESPUESTA_OK){
		respuesta = initProgramaUMC(pid, cantPaginas);
	} else {
		respuesta = string_itoa(RESPUESTA_FAIL);
	}

	free(codFuente);

	if (send(nucleo_socket, respuesta, RESPUESTA_SIZE, 0) == -1) {
		perror("send");
		exit(1);
	}

	return 0;
}

char* recibirFinalizarPrograma(int nucleo_socket){
	int32_t pid;
	char* respuesta;

	if (recv(nucleo_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	respuesta = finalizarProgramaSwap(&pid);

	if (send(nucleo_socket, respuesta, RESPUESTA_SIZE, 0) == -1) {
		perror("send");
		exit(1);
	}

	return 0;

}
