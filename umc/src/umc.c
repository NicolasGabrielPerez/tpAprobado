#include "umc-structs.h"
#include "swap-interfaz.h"
#include "cpu-interfaz.h"
#include "nucleo-interfaz.h"
#include "tlb.h"
#include "console-umc.h"

pthread_attr_t nucleo_attr;
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
			continue;
		}
		if(message->header == HEADER_SOLICITAR_PAGINAS){
			log_trace(logger, "HEADER_SOLICITAR_PAGINAS recibido [Socket %d]", (int) socket);
			recibirSolicitarPaginas((int)socket, pidActivo);
			continue;
		}
		if(message->header == HEADER_CAMBIO_PROCESO_ACTIVO){
			log_trace(logger, "HEADER_CAMBIO_PROCESO_ACTIVO recibido [Socket %d]", (int) socket);
			recibirCambioDeProcesoActivo((int)socket, &pidActivo);
			continue;
		}

		//TODO deleteMessage(response);
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
		creacion = pthread_create(&newThread, &nucleo_attr, &gestionarNucleo, (void*) new_socket);
	}
	if(tipo==TIPO_CPU){
		creacion = pthread_create(&newThread, &nucleo_attr, &gestionarCPU, (void*) new_socket);
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

int console_makeHandshake(int socket){
	message* message = receiveMessage(socket);

	if(message->codError == SOCKET_DESCONECTADO){
		log_error(logger, "Error haciendo handshake [Socket %d]", socket);
		return -1;
	}

	int tipo = convertToInt32(message->contenido);

	if(tipo == TIPO_NUCLEO){
		log_trace(logger, "Nuevo conexion de NUCLEO [Socket %d]", socket);
		return TIPO_NUCLEO;
	}
	if(tipo == TIPO_CPU){
		log_trace(logger, "Nuevo conexion de CPU [Socket %d]", socket);
		return TIPO_CPU;
	}

	log_error(logger, "Tipo desconocido [Socket %d]", socket);

	return -1;
}

void manejarNuevasConexiones(){
	int new_socket = aceptarNuevaConexion(listener);
	int tipo = console_makeHandshake(new_socket);

	if(tipo != -1){
		crearHiloDeComponente(tipo, new_socket);
	}
}

int main(void) {
	t_config* config = config_create("umc.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}

	initLogger();

	initMemoriaPrincipal(config);

	initTLB(config);

	initSwap(config);

	pthread_attr_init(&nucleo_attr);
	pthread_attr_setdetachstate(&nucleo_attr, PTHREAD_CREATE_DETACHED);

	char* puerto_cpu_nucleo = config_get_string_value(config, "PUERTO_CPU_NUCLEO");
	log_info(logger, "Iniciando puerto escucha para Cpu y Nucleo [Puerto %s]...", puerto_cpu_nucleo);
	listener = crear_puerto_escucha(puerto_cpu_nucleo);

	config_destroy(config);

	initUmcConsole();

	while(1){
		printf("Esperando conexiones...\n");
		manejarNuevasConexiones(); //Nucleo o CPU
	}

	close(listener); // bye!
	printf("Terminé felizmente");
	return EXIT_SUCCESS;
}
