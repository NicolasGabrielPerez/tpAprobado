#include "cpu-interfaz.h"
#include "swap-interfaz.h"

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

	//buscar en Tabla de Paginas del pid
	tabla_de_paginas_entry* entrada = buscarPorNroPaginaYPID(nroPagina, pidActivo);
	if(entrada==NULL){
		enviarFAIL(cpu_socket, PAGINA_NO_EXISTE);
	}
	if(entrada->presente){
		// esta en tabla de paginas ⇒ ir a buscar a Memoria Principal
		escribirEnFrame(buffer, offset, tamanio, entrada->nroFrame);
		entrada->modificado = 1;
		// y actualizar TLB
		if(TLBEnable) actualizarTLB(nroPagina, pidActivo);
	}

	// no esta ⇒ pedir a Swap
	response* paginaSolicitadaResult = pedirPaginaASwap(nroPagina, pidActivo);
	if(!paginaSolicitadaResult->ok){ //hubo error
		enviarResultado(paginaSolicitadaResult, cpu_socket);
		return;
	}

	// TODO cargar pagina y actualizar tabla de paginas
	// reemplazar si es necesario (Clock y clock modificado)
	cargarPagina(nroPagina, pidActivo, paginaSolicitadaResult->contenido);
	escribirEnFrame(buffer, offset, tamanio, entrada->nroFrame);

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

	// else: buscar en Tabla de Paginas del pid
	tabla_de_paginas_entry* entrada = buscarPorNroPaginaYPID(nroPagina, pidActivo); //esta busqueda seria penalizada
	if(entrada==NULL){
		enviarFAIL(cpu_socket, PAGINA_NO_EXISTE);
		return;
	}

	if(entrada->presente){
		// esta en tabla de paginas ⇒ ir a buscar a Memoria Principal
		bytesAEnviar = obtenerBytesDeMemoriaPrincipal(entrada->nroFrame, offset, tamanio);
		if(TLBEnable) actualizarTLB(nroPagina, pidActivo);
		enviarOKConContenido(cpu_socket, tamanio, bytesAEnviar);
		return;
	}

	// no esta ⇒ pedir a Swap
	response* paginaSolicitadaResult = pedirPaginaASwap(nroPagina, pidActivo);
	if(!paginaSolicitadaResult->ok){ //hubo error
		enviarResultado(paginaSolicitadaResult, cpu_socket);
		return;
	}

	// TODO cargar pagina y actualizar tabla de paginas
	// reemplazar si es necesario (Clock y clock modificado)
	cargarPagina(nroPagina, pidActivo, paginaSolicitadaResult->contenido);
	bytesAEnviar = obtenerBytes(paginaSolicitadaResult->contenido, offset, tamanio);
	//Actualizar TLB
	if(TLBEnable) actualizarTLB(nroPagina, pidActivo);

	enviarOKConContenido(cpu_socket, paginaSolicitadaResult->contenidoSize, paginaSolicitadaResult->contenido);
}

void recibirCambioDeProcesoActivo(int cpu_socket, int* pidActivo){
	int32_t pid;
	if (recv(cpu_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}
	tabla_de_paginas* tablaDePaginas = buscarPorPID(pidActivo);
	if(tablaDePaginas==NULL){
		enviarFAIL(cpu_socket, PID_NO_EXISTE);
		return;
	}
	*pidActivo = pid;
}
