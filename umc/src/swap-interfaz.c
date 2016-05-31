#include "swap-interfaz.h"

int swap_socket;
pthread_mutex_t swap_semaphore = PTHREAD_MUTEX_INITIALIZER;
//TODO usar esta variable desde la push-library
int32_t RESPUESTA_SIZE = sizeof(int32_t);

void initSwap(t_config* config){
	//char* ip_swap = config_get_string_value(config, "IP_SWAP");
	char* puerto_swap = config_get_string_value(config, "PUERTO_SWAP"); //puerto escucha de swap
	swap_socket = crear_socket_cliente("utnso40", puerto_swap);
	//char* cantPaginas = string_itoa(marco_size);
	//handshake(swap_socket, marco_size);
}

char* initProgramaSwap(int* pid, int* cantPaginas, char* codFuente){
	pthread_mutex_lock(swap_semaphore);
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
	pthread_mutex_unlock(swap_semaphore);
	return respuestaSwap;
}

char* finalizarProgramaSwap(int* pid){
	pthread_mutex_lock(swap_semaphore);
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
	pthread_mutex_unlock(swap_semaphore);
	return respuestaSwap;
}

char* pedirPaginaASwap(int nroPagina, int pid){
	pthread_mutex_lock(swap_semaphore);
	if (send(swap_socket, &HEADER_SOLICITAR_PAGINAS, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(swap_socket, nroPagina, sizeof(int32_t), 0) == -1) {
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
	int32_t* respuestaInt = malloc(sizeof(int32_t));
	memcpy(respuestaInt, respuestaSwap, sizeof(int32_t));
	if(respuestaInt==RESPUESTA_OK){
		char* pagina = malloc(marco_size);
		if (recv(swap_socket, pagina, marco_size, 0) == -1) {
			perror("recv");
			exit(1);
		}
		return pagina;
	} else{
		return string_itoa(RESPUESTA_FAIL);
	}
	pthread_mutex_unlock(swap_semaphore);
	return respuestaSwap;
}
