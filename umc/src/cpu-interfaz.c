#include "cpu-interfaz.h"

#include "clock.h"
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

	if(algoritmoClockEnable){
		response* clockResponse = clockGetFrame(nroPagina, pidActivo);
		if(!clockResponse->ok){
			enviarFAIL(cpu_socket, clockResponse->codError);
		}
		memcpy(&nroFrame, clockResponse->contenido, sizeof(int32_t));
	} else{
		//implementar clock modificado
	}

	escribirEnFrame(buffer, offset, tamanio, nroFrame);

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

	char* pagina;
	int nroFrame; //nroFrame a leer
	//TODO buscar en la tabla de paginas

	if(nroFrame != -1){ //si ya esta la pagina presente
		pagina = leerFrame(nroFrame);
		goto retornar;
	}

	tabla_de_paginas_entry* victima_entry;
	if(algoritmoClockEnable){
		//implementar clock
	} else{
		victima_entry = clockModificadoGetVictima(tablaDePaginas);
	}

	if(victima_entry->modificado){
		char* paginaModificada = leerFrame(victima_entry->nroFrame);
		escribirPaginaEnSwap(victima_entry->nroPagina, pidActivo, paginaModificada);
	}

	response* pedidoSwap = pedirPaginaASwap(nroPagina, pidActivo);
	pagina = pedidoSwap->contenido;

	retornar:
		//Actualizar TLB
		if(TLBEnable) actualizarTLB(nroPagina, pidActivo);

		enviarOKConContenido(cpu_socket, marco_size, pagina);
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
