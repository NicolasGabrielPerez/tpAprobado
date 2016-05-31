#include "swap-interfaz.h"

int swap_socket;
//TODO usar esta variable desde la push-library
int32_t RESPUESTA_SIZE = sizeof(int32_t);

void initSwap(t_config* config){
	char* ip_swap = config_get_string_value(config, "IP_SWAP");
	char* puerto_swap = config_get_string_value(config, "PUERTO_SWAP"); //puerto escucha de swap
	swap_socket = crear_socket_cliente("utnso40", puerto_swap);
	//char* cantPaginas = string_itoa(marco_size);
	//handshake(swap_socket, marco_size);
}

char* initProgramaSwap(int* pid, int* cantPaginas, char* codFuente){
	if (send(swap_socket, &HEADER_INIT_PROGRAMA, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(swap_socket, pid, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(swap_socket, cantPaginas, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(swap_socket, codFuente, (*cantPaginas)*marco_size, 0) == -1) {
		perror("send");
		exit(1);
	}
	char* respuestaSwap = malloc(RESPUESTA_SIZE);
	if (recv(swap_socket, respuestaSwap, RESPUESTA_SIZE, 0) == -1) {
		perror("recv");
		exit(1);
	}
	return respuestaSwap;
}

char* finalizarProgramaSwap(int* pid){
	if (send(swap_socket, &HEADER_FIN_PROGRAMA, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(swap_socket, pid, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	char* respuestaSwap = malloc(RESPUESTA_SIZE);
	if (recv(swap_socket, respuestaSwap, RESPUESTA_SIZE, 0) == -1) {
		perror("recv");
		exit(1);
	}
	return respuestaSwap;
}

char* pedirPaginaASwap(int nroPagina, int pid){
	return 0;
}
