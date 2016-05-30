#include "interfazUmc.h"

int umc_socket;

int esperarConexionUMC(int umc_listener){
	int umc_socket;
	int bytes_recibidos;
	char* header = malloc(HEADER_SIZE);
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;
	char remoteIP[INET6_ADDRSTRLEN];

	   // handle new connections
	   addrlen = sizeof remoteaddr;
	   umc_socket = accept(umc_listener,
		   (struct sockaddr *)&remoteaddr,
		   &addrlen);

	   puts("Conexion aceptada");
	   if (umc_socket == -1) {
		   perror("accept");
	   } else {

			printf("umc: new connection from %s on "
				"socket %d\n",
				inet_ntop(remoteaddr.ss_family,
					get_in_addr((struct sockaddr*)&remoteaddr),
					remoteIP, INET6_ADDRSTRLEN),
					umc_listener);

			printf("El fd es: %d", umc_socket);
			if ((bytes_recibidos = recv(umc_socket, header, HEADER_SIZE, 0)) == -1) {
			   perror("recv");
			   exit(1);
		   }

			makeHandshake();
	   }

	   free(header);
	   return umc_socket;
}

int initUmc(t_config* config){
	char* puerto_umc = config_get_string_value(config, "PUERTO_UMC");
	int umc_listener = crear_puerto_escucha(puerto_umc);
	umc_socket = esperarConexionUMC(umc_listener);
	return EXIT_SUCCESS;
}

void recibirInitPrograma(){
	int32_t pid;
	int32_t cantPaginas;
	char* codFuente;
	char* respuesta;

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
		respuesta = string_itoa(RESPUESTA_FAIL);
		goto enviarRespuesta;
	}

	escribirPaginas(pid, cantPaginas, codFuente);
	respuesta = string_itoa(RESPUESTA_OK);

	enviarRespuesta:
		if (send(umc_socket, respuesta, sizeof(int32_t), 0) == -1) {
				perror("send");
				exit(1);
		}

}

void recibirPedidoPagina(){
	int32_t nroPagina;
	int32_t pid;
	char* respuesta;

	if (recv(umc_socket, &nroPagina, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if (recv(umc_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	respuesta = buscarPagina(nroPagina, pid);

	if (send(umc_socket, respuesta, paginaSize, 0) == -1) {
			perror("send");
			exit(1);
	}
}

void recibirEscrituraPagina(){
	int32_t nroPagina;
	int32_t pid;
	char* buffer = malloc(paginaSize);
	char* respuesta;

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

	respuesta = escribirPagina(nroPagina, pid, buffer);

	if (send(umc_socket, respuesta, paginaSize, 0) == -1) {
			perror("send");
			exit(1);
	}
}

void recibirFinPrograma(){

}

void makeHandshake(){

}

