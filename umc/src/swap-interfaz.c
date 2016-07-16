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
	log_info(logger, "Swap IP: %s", ip_swap);
	log_info(logger, "Swap port: %s", puerto_swap);

	log_info(logger, "Contectandose con Swap.....................");
	swap_socket = crear_socket_cliente(ip_swap, puerto_swap);
	log_info(logger, "Conexion con Swap realizada");

	response* swapInitResponse = handshakeSwap();

	if(swapInitResponse->ok){
		log_info(logger, "Respuesta incial de Swap: %s\n", swapInitResponse->contenido);
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

response* finalizarProgramaSwap(int* pid){
	pthread_mutex_lock(&swap_semaphore);
	log_trace(logger, "Enviando fin de pid %d a Swap...", pid);
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
	response* swapResponse = recibirResponse(swap_socket);
	log_trace(logger, "Obtenida respuesta de Swap");
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
