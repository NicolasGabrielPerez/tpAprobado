#include "cpu-interfaz.h"

void recibirAlmacenarPaginas(int cpu_socket, int pidActivo){
	int32_t nroPagina;
	int32_t offset;
	int32_t tamanio;
	char* buffer;

	message* nroPaginaMessage = receiveMessage(cpu_socket);
	nroPagina = convertToInt32(nroPaginaMessage->contenido);

	message* offsetMessage = receiveMessage(cpu_socket);
	offset = convertToInt32(offsetMessage->contenido);

	message* tamanioMessage = receiveMessage(cpu_socket);
	tamanio = convertToInt32(tamanioMessage->contenido);

	message* bufferMessage = receiveMessage(cpu_socket);
	buffer = bufferMessage->contenido;

	deleteMessage(nroPaginaMessage);
	deleteMessage(offsetMessage);
	deleteMessage(tamanioMessage);

	log_trace(logger, "Pedido almacenar paginas [pid: %d, nroPagina %d, offset %d, tamanio %d]", pidActivo, nroPagina, offset, tamanio);
	log_trace(logger, "[Buffer: %s]", buffer);

	//Esto es solo una validacion
	tabla_de_paginas* tablaDePaginas = buscarPorPID(pidActivo);
	if(tablaDePaginas==NULL){
		log_error(logger, "Pid %d no existe", pidActivo);
		enviarFAIL(cpu_socket, PID_NO_EXISTE);
		log_trace(logger, "Enviada respuesta de fallo");

		deleteMessage(bufferMessage);
		return;
	}

	if(TLBEnable){
		int frame = buscarEnTLB(nroPagina, pidActivo);
		if(frame!=-1){
			escribirEnFrame(buffer, offset, tamanio, frame);
			enviarOKSinContenido(cpu_socket);

			deleteMessage(bufferMessage);
			return;
		}
	}

	demorarSolicitud();
	umcResult result = getPageEntry(tablaDePaginas, nroPagina);
	if(!result.ok){
		enviarFAIL(cpu_socket, result.codError);
		log_trace(logger, "Enviada respuesta de fallo");
		return;
	}

	escribirEnFrame(buffer, offset, tamanio, result.frameEntry->nroFrame);

	//Actualizar TLB
	if(TLBEnable) actualizarTLB(nroPagina, pidActivo, result.frameEntry->nroFrame);

	demorarSolicitud();
	enviarOKSinContenido(cpu_socket);

	deleteMessage(bufferMessage);

	log_trace(logger, "Enviada respuesta Ok! [Socket %d]", cpu_socket);
}

void recibirSolicitarPaginas(int cpu_socket, int pidActivo){
	char* bytesAEnviar;
	int32_t nroPagina;
	int32_t offset;
	int32_t tamanio;

	message* nroPaginaMessage = receiveMessage(cpu_socket);
	nroPagina = convertToInt32(nroPaginaMessage->contenido);

	message* offsetMessage = receiveMessage(cpu_socket);
	offset = convertToInt32(offsetMessage->contenido);

	message* tamanioMessage = receiveMessage(cpu_socket);
	tamanio = convertToInt32(tamanioMessage->contenido);


	deleteMessage(nroPaginaMessage);
	deleteMessage(offsetMessage);
	deleteMessage(tamanioMessage);

	log_trace(logger, "Pedido recibir paginas [pid: %d, nroPagina %d, offset %d, tamanio %d]", pidActivo, nroPagina, offset, tamanio);

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
	//TLB Miss

	demorarSolicitud();
	umcResult result = getPageEntry(tablaDePaginas, nroPagina);
	if(!result.ok){
		enviarFAIL(cpu_socket, result.codError);
	}

	//Actualizar TLB
	if(TLBEnable) actualizarTLB(nroPagina, pidActivo, result.frameEntry->nroFrame);

	demorarSolicitud();
	enviarOKConContenido(cpu_socket, tamanio, result.frameEntry->direccion_real + offset);
}

void recibirCambioDeProcesoActivo(int cpu_socket, int* pidActivo){
	int32_t pid;
	message* pidMessage = receiveMessage(cpu_socket);
	pid = convertToInt32(pidMessage->contenido);

	log_trace(logger, "Pedido cambio pid activo [pid: %d]", pid);

	tabla_de_paginas* tablaDePaginas = buscarPorPID(pid);
	if(tablaDePaginas==NULL){
		log_error(logger, "El pid %d no existe", pid);
		enviarFAIL(cpu_socket, PID_NO_EXISTE);
		log_trace(logger, "Enviada respuesta de fallo");
		return;
	}
	if(TLBEnable){
		flush(*pidActivo);
	}
	*pidActivo = pid;

	enviarOKSinContenido(cpu_socket);
	log_trace(logger, "Enviada respuesta Ok");
}
