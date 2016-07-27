#include "umc-structs.h"
#include "swap-interfaz.h"
#include "cpu-interfaz.h"
#include "nucleo-interfaz.h"
#include "tlb.h"
#include "console-umc.h"
#include <sockets/communication.h>

pthread_attr_t pthread_attr;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int listener;

void *gestionarCPU(void* socket){
	log_trace(logger, "Creado hilo de gestion CPU [Socket %d]", (int) socket);

	int pidActivo = 0;

	while(1){

		log_info(logger, "Esperando mensajes de [Socket %d]", (int) socket);
		message* message = receiveMessage((int) socket);

		if(message->codError == SOCKET_DESCONECTADO){
			log_warning(logger, "Socket de CPU %d desconectado\n", (int) socket);
			return 0;
		}

		if(message->header == HEADER_ALMACENAR_PAGINAS){
			log_trace(logger, "HEADER_ALMACENAR_PAGINAS recibido [Socket %d]", (int) socket);
			recibirAlmacenarPaginas((int)socket, pidActivo);
		} else if(message->header == HEADER_SOLICITAR_PAGINAS){
			log_trace(logger, "HEADER_SOLICITAR_PAGINAS recibido [Socket %d]", (int) socket);
			recibirSolicitarPaginas((int)socket, pidActivo);
		} else if(message->header == HEADER_CAMBIO_PROCESO_ACTIVO){
			log_trace(logger, "HEADER_CAMBIO_PROCESO_ACTIVO recibido [Socket %d]", (int) socket);
			recibirCambioDeProcesoActivo((int)socket, &pidActivo);
		}

		deleteMessage(message);
	}

	return 0;
}

void *gestionarNucleo(void* socket){
	log_trace(logger, "Creado hilo de gestion NUCLEO [Socket %d]", (int) socket);

	while(1){

		log_info(logger, "Esperando mensajes de [Socket %d]", (int) socket);
		message* message = receiveMessage((int) socket);

		if(message->codError == SOCKET_DESCONECTADO){
			log_warning(logger, "Socket de NUCLEO %d desconectado\n", (int) socket);
			return 0;
		}

		if(message->header == HEADER_INIT_PROGRAMA){
			log_trace(logger, "HEADER_INIT_PROGRAMA recibido");
			recbirInitPrograma((int)socket);
			continue;
		}
		if(message->header == HEADER_FIN_PROGRAMA){
			log_trace(logger, "HEADER_FIN_PROGRAMA recibido");
			recibirFinalizarPrograma((int)socket);
			continue;
		}

		//TODO deleteMessage(response);
	}

	return 0;
}

int crearHiloDeComponente(int tipo, int new_socket){
	pthread_t newThread;
	int creacion;
	if(tipo==TIPO_NUCLEO){
		creacion = pthread_create(&newThread, &pthread_attr, &gestionarNucleo, (void*) new_socket);
	}
	if(tipo==TIPO_CPU){
		creacion = pthread_create(&newThread, &pthread_attr, &gestionarCPU, (void*) new_socket);
	}
	return creacion;
}

response* recibir(int socket, int size){
	char* header = malloc(HEADER_SIZE);
	int bytesReceived = recv(socket, header, HEADER_SIZE, 0);
	if(bytesReceived == 0) {
		return createFAILResponse(SOCKET_DESCONECTADO);
	}
	if(bytesReceived == -1){
		return createFAILResponse(SOCKET_ERROR_DESCONOCIDO);
	}
	return createResponse(1,0,HEADER_SIZE,header);
}

void enviarPageSize(int socket){
	char* pageSizeSerializado = malloc(sizeof(int32_t));
	serializarInt(pageSizeSerializado, &marco_size);
	sendMessage(socket, HEADER_SIZE, sizeof(int32_t), pageSizeSerializado);
	free(pageSizeSerializado);
}

int makeHandshake(int socket){
	message* message = receiveMessage(socket);

	if(message->codError == SOCKET_DESCONECTADO){
		log_error(logger, "Error haciendo handshake [Socket %d]", socket);
		return -1;
	}

	int tipo = convertToInt32(message->contenido);

	if(tipo == TIPO_NUCLEO){
		log_trace(logger, "Nuevo conexion de NUCLEO [Socket %d]", socket);
		sendMessageInt(socket, HEADER_HANDSHAKE, marco_size);
		return TIPO_NUCLEO;
	}
	if(tipo == TIPO_CPU){
		log_trace(logger, "Nuevo conexion de CPU [Socket %d]", socket);
		sendMessageInt(socket, HEADER_HANDSHAKE, marco_size);
		return TIPO_CPU;
	}

	log_error(logger, "Tipo desconocido [Socket %d]", socket);

	return -1;
}

void manejarNuevasConexiones(){
	int new_socket = aceptarNuevaConexion(listener);
	int tipo = makeHandshake(new_socket);

	if(tipo != -1){
		crearHiloDeComponente(tipo, new_socket);
	}
}

void initPid(int pid, int cantPaginas, char* codFuente){

	response* swapResponse = initProgramaSwap(&pid, &cantPaginas, codFuente);

	if(swapResponse->ok){
		response* umcInitPidResponse = initProgramaUMC(pid, cantPaginas);
		if(!umcInitPidResponse->ok){
			log_error(logger, "Error");
			return;
		}
		log_trace(logger, "Enviada respuesta OK! de init pid %d", pid);
	}

	if(!swapResponse->ok){
		log_warning(logger, "Respuesta de fallo: Error %d", swapResponse->codError);
	}
}

char* solicitarPagina(int nroPaginaSolicitada, int offset, int size, int pid){
	//Esto es solo una validacion
	tabla_de_paginas* tablaDePaginas = buscarPorPID(pid);
	if(tablaDePaginas==NULL){
		return NULL;
	}

	if(TLBEnable){
		//Buscar en TLB. Si esta, ir a buscar a memoria y enviar bytes
		int frame = buscarEnTLB(nroPaginaSolicitada, pid);
		if(frame!=-1){
			return obtenerBytesDeMemoriaPrincipal(frame, offset, size);
		}
	}

	umcResult result = getPageEntry(tablaDePaginas, nroPaginaSolicitada);
	if(!result.ok){
		return NULL;
	}

	//Actualizar TLB
	if(TLBEnable) actualizarTLB(nroPaginaSolicitada, pid, result.frameEntry->nroFrame);

	char* contenido = malloc(size);
	memcpy(contenido, result.frameEntry->direccion_real + offset, size);
	return contenido;
}

int escribirEnPagina(int nroPagina, int offset, int size, char* buffer, int pid){
	//Esto es solo una validacion
	tabla_de_paginas* tablaDePaginas = buscarPorPID(pid);
	if(tablaDePaginas==NULL){
		log_error(logger, "Pid %d no existe", pid);
		return -1;
	}


	char* escritura;

	if(buffer == NULL){
		escritura = "XXYYZZ";
	} else{
		escritura = buffer;
	}

	if(TLBEnable){
		int frame = buscarEnTLB(nroPagina, pid);
		if(frame!=-1){
			escribirEnFrame(escritura, offset, size, frame);
			return 1;
		}
	}

	umcResult result = getPageEntry(tablaDePaginas, nroPagina);
	if(!result.ok){
		return -1;
		log_trace(logger, "Enviada respuesta de fallo");
	}

	escribirEnFrame(escritura, offset, size, result.frameEntry->nroFrame);

	//Actualizar TLB
	if(TLBEnable) actualizarTLB(nroPagina, pid, result.frameEntry->nroFrame);

	log_trace(logger, "Pagina escrita!");
	return 1;
}

void finalizarPid(int pid){
	response* result = finalizarPidDeUMC(pid);

	if(!result->ok){
		printf("FATAL ERROR");
		return;
	}

	response* swapResponse = finalizarProgramaSwap(pid);
}

void testFragmentacionExterna(){

	int pid11 = 11;
	int cantPaginasPorPrograma = 5;

	char* codFuente1 = "#!/usr/bin/ansisop\n"
				"begin\n"
				"variables a, b\n"
				"a = 3\n"
				"b = 5\n"
				"a = b + 12\n"
				"end";

	initPid(pid11, cantPaginasPorPrograma, codFuente1);

	int pid22 = 22;
	initPid(pid22, cantPaginasPorPrograma, codFuente1);

	int pid33 = 33;
	initPid(pid33, cantPaginasPorPrograma, codFuente1);

	//Hasta aca deberia haber 3 pid ocupando 15 frames en swap

	finalizarPid(pid22);

	//Ahora 2 pid ocupando 10 frames en swap => Espacio disponible en swap = 6

	char* codFuente6Pages = "#!/usr/bin/ansisop\n"
				"begin\n"
				"variables a, b\n"
				"a = 3\n"
				"b = 5\n"
				"a = b + 12\n"
				"end"
				"12345678";
	initPid(50, 6, codFuente6Pages);

	int nroPaginaSolicitada = 2;
	int offset = 0;
	int pageSize = 8;

	char* pagina0 = solicitarPagina(0, 0, pageSize, pid11);
	pagina0[pageSize] = '\0';
	printf("paginaSolicitada1: %s\n", pagina0);

}

int main(void) {

	t_config* config = config_create("umc.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}

	initLogger();

	initMemoriaPrincipal(config);

	initSwapConsole();

	initTLB(config);

	initSwap(config);

	pthread_attr_init(&pthread_attr);
	pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED);

	char* puerto_cpu_nucleo = config_get_string_value(config, "PUERTO_CPU_NUCLEO");
	log_info(logger, "Iniciando puerto escucha para Cpu y Nucleo [Puerto %s]...", puerto_cpu_nucleo);
	listener = crear_puerto_escucha(puerto_cpu_nucleo);

	config_destroy(config);

	while(1){
		printf("Esperando conexiones...\n");
		manejarNuevasConexiones(); //Nucleo o CPU
	}

	close(listener); // bye!
	printf("Terminé felizmente");
	return EXIT_SUCCESS;
}
