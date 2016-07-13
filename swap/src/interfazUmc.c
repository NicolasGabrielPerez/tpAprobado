#include "interfazUmc.h"

int umc_socket;

int esperarConexionUMC(int umc_listener){
	return aceptarNuevaConexion(umc_listener);
}

int initUmc(t_config* config){
	char* puerto_umc = config_get_string_value(config, "PUERTO_UMC");
	int umc_listener = crear_puerto_escucha(puerto_umc);
	umc_socket = esperarConexionUMC(umc_listener);

	log_trace(logger, "Conexion iniciada con UMC. Socket: %d", umc_socket);

	return EXIT_SUCCESS;
}

void recibirInitPrograma(){
	int32_t pid;
	int32_t cantPaginas;
	char* codFuente;

	if (recv(umc_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if (recv(umc_socket, &cantPaginas, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	codFuente = malloc(paginaSize * cantPaginas);

	if (recv(umc_socket, codFuente, paginaSize * cantPaginas, 0) == -1) {
		perror("recv");
		exit(1);
	}

	if(!hayEspacioDisponible(cantPaginas)){
		enviarFAIL(umc_socket, ESPACIO_NO_DISPONIBLE);
		return;
	}
	if(existePid(pid)){
		enviarFAIL(umc_socket, PID_YA_EXISTE);
		return;
	}

	initPaginas(pid, cantPaginas, codFuente);

	enviarOKSinContenido(umc_socket);
}

void recibirPedidoPagina(){
	int32_t nroPagina;
	int32_t pid;

	if (recv(umc_socket, &nroPagina, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if (recv(umc_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	response* paginaResult = getPagina(nroPagina, pid);
	enviarResultado(paginaResult, umc_socket);
}

void recibirEscrituraPagina(){
	int32_t nroPagina;
	int32_t pid;
	char* buffer = malloc(paginaSize);

	if (recv(umc_socket, &nroPagina, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if (recv(umc_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if (recv(umc_socket, buffer, paginaSize, 0) == -1) {
		perror("recv");
		exit(1);
	}

	response* escrituraResult = escribirPagina(nroPagina, pid, buffer);
	enviarResultado(escrituraResult, umc_socket);
}

void recibirFinPrograma(){
	int32_t pid;
	char* respuesta = malloc(RESPUESTA_SIZE);

	if (recv(umc_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if(!existePid(pid)){
		respuesta = string_itoa(RESPUESTA_FAIL);
		if (send(umc_socket, respuesta, sizeof(int32_t), 0) == -1) {
				perror("send");
				exit(1);
		};
		return;
	}

	response* finalizarResult = finalizarPrograma(pid);
	enviarResultado(finalizarResult, umc_socket);
}

void makeHandshake(){
	char mensajeInicial[10] = "Soy Swap!";
	enviarOKConContenido(umc_socket, sizeof(mensajeInicial), mensajeInicial);
}

