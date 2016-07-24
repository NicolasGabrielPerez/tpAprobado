#include "swap-interfaz.h"
int swap_socket;
pthread_mutex_t swap_semaphore = PTHREAD_MUTEX_INITIALIZER;

response* handshakeSwap(){
	printf("Haciendo handshake con Swap...\n");
	if (send(swap_socket, &HEADER_HANDSHAKE, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	return recibirResponse(swap_socket);
}

void initSwap(t_config* config){
	char* ip_swap = config_get_string_value(config, "IP_SWAP");
	char* puerto_swap = config_get_string_value(config, "PUERTO_SWAP");

	log_info(logger, "Contectandose con Swap...[Swap IP: %s, Swap port: %s]", ip_swap, puerto_swap);
	swap_socket = crear_socket_cliente(ip_swap, puerto_swap);

	response* swapInitResponse = handshakeSwap();

	if(swapInitResponse->ok){
		log_info(logger, "Respuesta incial de Swap: %s", swapInitResponse->contenido);
		return;
	} else{
		log_error(logger, "Error de Swap: %d", swapInitResponse->codError);
	}

	exit(1);
}

response* initProgramaSwap(int* pid, int* cantPaginas, char* codFuente){
	//pthread_mutex_lock(&swap_semaphore);
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
	response* swapResponse = recibirResponse(swap_socket);
	//pthread_mutex_unlock(&swap_semaphore);
	return swapResponse;
}

response* finalizarProgramaSwap(int pid){
	pthread_mutex_lock(&swap_semaphore);
	log_trace(logger, "Enviando fin de pid %d a Swap...", pid);
	if (send(swap_socket, &HEADER_FIN_PROGRAMA, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(swap_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}

	response* swapResponse = recibirResponse(swap_socket);
	if(swapResponse->ok){
		log_info(logger, "Obtenida respuesta OK de Swap");
	} else{
		log_warning(logger, "Obtenido error %d de Swap", swapResponse->codError);
	}

	pthread_mutex_unlock(&swap_semaphore);
	return swapResponse;
}

response* pedirPaginaASwap(int nroPagina, int pid){
	pthread_mutex_lock(&swap_semaphore);
	log_trace(logger, "Enviando pedido de pagina a Swap...");
	log_trace(logger, "Pedido [pid %d, nroPagina %d]", pid, nroPagina);
	if (send(swap_socket, &HEADER_SOLICITAR_PAGINAS, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(swap_socket, &nroPagina, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(swap_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	response* swapResponse = recibirResponse(swap_socket);
	log_trace(logger, "Obtenida respuesta de Swap");
	pthread_mutex_unlock(&swap_semaphore);
	return swapResponse;
}

response* escribirPaginaEnSwap(int nroPagina, int pid, char* contenido){
	pthread_mutex_lock(&swap_semaphore);
	if (send(swap_socket, &HEADER_ALMACENAR_PAGINAS, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(swap_socket, &nroPagina, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(swap_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(swap_socket, contenido, marco_size, 0) == -1) {
		perror("send");
		exit(1);
	}
	response* swapResponse = recibirResponse(swap_socket);
	pthread_mutex_unlock(&swap_semaphore);
	return swapResponse;
}
