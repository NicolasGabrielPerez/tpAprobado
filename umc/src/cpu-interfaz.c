#include "cpu-interfaz.h"

#include "pages_replacement.h"
#include "umc-structs.h"

void recibirAlmacenarPaginas(int cpu_socket, int pidActivo){
	int32_t nroPagina;
	int32_t offset;
	int32_t tamanio;
	char* buffer;

	if (recv(cpu_socket, &nroPagina, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if (recv(cpu_socket, &offset, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if (recv(cpu_socket, &tamanio, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	buffer = malloc(tamanio);
	if (recv(cpu_socket, buffer, tamanio, 0) == -1) {
		perror("recv");
		exit(1);
	}

	//Esto es solo una validacion
	tabla_de_paginas* tablaDePaginas = buscarPorPID(pidActivo);
	if(tablaDePaginas==NULL){
		enviarFAIL(cpu_socket, PID_NO_EXISTE);
		return;
	}

	if(TLBEnable){
		int frame = buscarEnTLB(nroPagina, pidActivo);
		if(frame!=-1){
			escribirEnFrame(buffer, offset, tamanio, frame);
			enviarOKSinContenido(cpu_socket);
			return;
		}
	}

	int nroFrame; //nroFrame a escribir

	umcResult result = getPageEntry(tablaDePaginas, nroPagina);
	if(!result.ok){
		enviarFAIL(cpu_socket, result.codError);
	}

	escribirEnFrame(result.frameEntry->direccion_real, offset, tamanio, nroFrame);

	//Actualizar TLB
	if(TLBEnable) actualizarTLB(nroPagina, pidActivo);

	enviarOKSinContenido(cpu_socket);
}

void recibirSolicitarPaginas(int cpu_socket, int pidActivo){
	char* bytesAEnviar;
	int32_t nroPagina;
	int32_t offset;
	int32_t tamanio;

	if (recv(cpu_socket, &nroPagina, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if (recv(cpu_socket, &offset, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if (recv(cpu_socket, &tamanio, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	//Esto es solo una validacion
	tabla_de_paginas* tablaDePaginas = buscarPorPID(pidActivo);
	if(tablaDePaginas==NULL){
		enviarFAIL(cpu_socket, PID_NO_EXISTE);
		return;
	}

	if(TLBEnable){
		//Buscar en TLB. Si esta, ir a buscar a memoria y enviar bytes
		int frame = buscarEnTLB(nroPagina, pidActivo);
		if(frame!=-1){
			bytesAEnviar = obtenerBytesDeMemoriaPrincipal(frame, offset, tamanio);
			enviarOKConContenido(cpu_socket, tamanio, bytesAEnviar);
			return;
		}
	}

	umcResult result = getPageEntry(tablaDePaginas, nroPagina);
	if(!result.ok){
		enviarFAIL(cpu_socket, result.codError);
	}

	retornar:
		//Actualizar TLB
		if(TLBEnable) actualizarTLB(nroPagina, pidActivo);

		enviarOKConContenido(cpu_socket, marco_size, result.frameEntry->direccion_real);
}

void recibirCambioDeProcesoActivo(int cpu_socket, int* pidActivo){
	int32_t pid;
	if (recv(cpu_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}
	tabla_de_paginas* tablaDePaginas = buscarPorPID(*pidActivo);
	if(tablaDePaginas==NULL){
		enviarFAIL(cpu_socket, PID_NO_EXISTE);
		return;
	}
	*pidActivo = pid;
}
