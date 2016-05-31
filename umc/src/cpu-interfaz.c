#include "cpu-interfaz.h"
#include "swap-interfaz.h"

void recibirAlmacenarPaginas(int cpu_socket, int pidActivo){
	int32_t nroPagina;
	int32_t offset;
	int32_t tamanio;
	char* buffer;
	char* respuesta;

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

	if(TLBEnable){
		//Buscar en TLB. Si esta, ir a buscar a memoria y enviar bytes
		int frame = buscarEnTLB(nroPagina, pidActivo);
		if(frame!=-1){
			escribirEnFrame(buffer, offset, tamanio, frame);
			respuesta = string_itoa(RESPUESTA_OK);
			goto enviarBytes;
		}
	}

	// else: buscar en Tabla de Paginas del pid
	tabla_de_paginas_entry* entrada = buscarPorNroPaginaYPID(nroPagina, pidActivo);
	if(entrada==NULL){
		respuesta = string_itoa(RESPUESTA_FAIL); //no existe tal nroPagina para tal pid
		goto enviarBytes;
	}
	if(entrada->presente){
		// esta en tabla de paginas ⇒ ir a buscar a Memoria Principal
		escribirEnFrame(buffer, offset, tamanio, entrada->nroFrame);
		// y actualizar TLB
		if(TLBEnable) actualizarTLB(nroPagina, pidActivo);
	}

	// no esta ⇒ pedir a Swap
	char* paginaSolicitada = pedirPaginaASwap(nroPagina, pidActivo);
	if(paginaSolicitada==NULL){
		respuesta = string_itoa(RESPUESTA_FAIL); //no existe tal nroPagina para tal pid
		goto enviarBytes;
	}
	// TODO cargar pagina y actualizar tabla de paginas
	// reemplazar si es necesario (Clock y clock modificado)
	cargarPagina(nroPagina, pidActivo, paginaSolicitada);
	escribirEnFrame(buffer, offset, tamanio, entrada->nroFrame);
	respuesta = string_itoa(RESPUESTA_OK);

	//Actualizar TLB
	if(TLBEnable) actualizarTLB(nroPagina, pidActivo);

	enviarBytes:
		if (send(cpu_socket, respuesta, tamanio, 0) == -1) {
				perror("send");
				exit(1);
		}
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

	if(TLBEnable){
		//Buscar en TLB. Si esta, ir a buscar a memoria y enviar bytes
		int frame = buscarEnTLB(nroPagina, pidActivo);
		if(frame!=-1){
			bytesAEnviar = obtenerBytesDeMemoriaPrincipal(frame, offset, tamanio);
			goto enviarBytes;
		}
	}

	// else: buscar en Tabla de Paginas del pid
	tabla_de_paginas_entry* entrada = buscarPorNroPaginaYPID(nroPagina, pidActivo);
	if(entrada==NULL){
		bytesAEnviar = string_itoa(RESPUESTA_FAIL); //no existe tal nroPagina para tal pid
		goto enviarBytes;
	}
	if(entrada->presente){
		// esta en tabla de paginas ⇒ ir a buscar a Memoria Principal
		bytesAEnviar = obtenerBytesDeMemoriaPrincipal(entrada->nroFrame, offset, tamanio);
		// y actualizar TLB
		if(TLBEnable) actualizarTLB(nroPagina, pidActivo);
	}

	// no esta ⇒ pedir a Swap
	char* paginaSolicitada = pedirPaginaASwap(nroPagina, pidActivo);
	if(paginaSolicitada==NULL){
		bytesAEnviar = string_itoa(RESPUESTA_FAIL); //no existe tal nroPagina para tal pid
		goto enviarBytes;
	}

	//TODO Primero enviar OK

	// TODO cargar pagina y actualizar tabla de paginas
	// reemplazar si es necesario (Clock y clock modificado)
	cargarPagina(nroPagina, pidActivo, paginaSolicitada);
	bytesAEnviar = obtenerBytes(paginaSolicitada, offset, tamanio);

	//Actualizar TLB
	if(TLBEnable) actualizarTLB(nroPagina, pidActivo);

	enviarBytes:
		if (send(cpu_socket, bytesAEnviar, tamanio, 0) == -1) {
				perror("send");
				exit(1);
		}
}

void recibirCambioDeProcesoActivo(int cpu_socket, int* pidActivo){
	int32_t pid;
	if (recv(cpu_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}
	*pidActivo = pid;
}
