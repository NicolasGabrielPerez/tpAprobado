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
	printf("Creado hilo de gestión de CPU\n");
	printf("De socket: %d\n", (int)socket);

	int pidActivo = 0;
	int32_t headerInt;
	char* header = malloc(HEADER_SIZE);
	while(1){
		if (recv((int)socket, header, HEADER_SIZE, 0) <=0) {
			printf("Socket de CPU %d desconectado\n", (int) socket);
			return 0;
		}
		memcpy(&headerInt, header, sizeof(int32_t));
		if(headerInt==HEADER_ALMACENAR_PAGINAS){
			recibirAlmacenarPaginas((int)socket, pidActivo);
			continue;
		}
		if(headerInt==HEADER_SOLICITAR_PAGINAS){
			recibirSolicitarPaginas((int)socket, pidActivo);
			continue;
		}
		if(headerInt==HEADER_CAMBIO_PROCESO_ACTIVO){
			recibirCambioDeProcesoActivo((int)socket, &pidActivo);
			continue;
		}
	}

	return 0;
}

void *gestionarNucleo(void* socket){
	printf("Creado hilo de gestión de Nucleo\n");
	printf("De socket: %d\n", (int)socket);

	while(1){

		message* message = receiveMessage((int) socket);

		if(message->codError == SOCKET_DESCONECTADO){
			printf("Socket de NUCLEO %d desconectado\n", (int) socket);
			return 0;
		}

		if(message->header == HEADER_INIT_PROGRAMA){
			log_trace(logger, "Init program received");
			recbirInitPrograma((int)socket);
			continue;
		}
		if(message->header == HEADER_FIN_PROGRAMA){
			log_trace(logger, "End program received");
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
	char* pageSizeSerializado;
	serializarInt(pageSizeSerializado, &marco_size);
	sendMessage(socket, HEADER_SIZE, sizeof(int32_t), pageSizeSerializado);
}

int makeHandshake(int socket){
	message* message = receiveMessage(socket);
	return convertToInt32(message->contenido);
}

void manejarNuevasConexiones(){
	int new_socket = aceptarNuevaConexion(listener);
	int tipo = makeHandshake(new_socket);
	if(tipo == -1){
		printf("Hubo error en handshake");
		return;
	}
	if(tipo == TIPO_NUCLEO || tipo == TIPO_CPU){
		if(tipo == TIPO_NUCLEO) printf("Nuevo conexion de tipo NUCLEO\n");
		if(tipo == TIPO_CPU) printf("Nuevo conexion de tipo CPU\n");
		enviarPageSize(new_socket);
	}

	crearHiloDeComponente(tipo, new_socket);
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

	char* puerto_cpu_nucleo = config_get_string_value(config, "PUERTO_CPU_NUCLEO"); //puerto escucha de Nucleo y CPU
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
