#include "umc-interfaz.h"

int inicializarPrograma(int program_id, int paginas_requeridas){
	return 0;
}

int solicitarBytesDePagina(int nro_pagina, int offset, int size){
	return 0;
}

int almacenarBytesEnPagina(int nro_pagina, int offset, int size, char* buffer){
	return 0;
}

int finalizarPrograma(int program_id){
	return 0;
}

int devolver_handshake(int socket, int tipo){

	if (send(socket, "Soy UMC", 50, 0) == -1) {
		 perror("send");
		 return -1;
	 }

	puts("Termino el handshake\n");
	return 0;
}

int recibir_handshake(int socket){
	int bytes_recibidos;
	char buf[50];
	int tipo = 1;

	printf("Iniciando handshake con socket: %d\n", socket);
	if ((bytes_recibidos = recv(socket, buf, 50, 0)) == -1) {
	   perror("recv");
	   return -1;
	}

	printf("Se recibio: %s\nbytes_recibidos: %d.\n", buf, bytes_recibidos);
	//tipo = *((int*)buf[0]);
	//free(buf);
	return 2;
}

int cambioDeProcesoActivo(int program_id){
	return 0;
}

