#include "umc-structs.h"
#include "swap-interfaz.h"
#include "cpu-interfaz.h"
#include "nucleo-interfaz.h"
#include "tlb.h"
#include "console-umc.h"

pthread_attr_t attr;
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
		if (recv((int)socket, header, HEADER_SIZE, 0) == -1) {
			perror("recv");
			exit(1);
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

	int32_t headerInt;
	char* header = malloc(HEADER_SIZE);
	while(1){
		if (recv((int)socket, header, HEADER_SIZE, 0) == -1) {
			perror("recv");
			exit(1);
		}
		memcpy(&headerInt, header, sizeof(int32_t));
		if(headerInt==HEADER_INIT_PROGRAMA){
			recbirInitPrograma((int)socket);
			continue;
		}
		if(headerInt==HEADER_FIN_PROGRAMA){
			recibirFinalizarPrograma((int)socket);
			continue;
		}
	}

	return 0;
}

int crearHiloDeComponente(int tipo, int new_socket){
	pthread_t newThread;
	int creacion;
	if(tipo==TIPO_NUCLEO){
		creacion = pthread_create(&newThread, &attr, &gestionarNucleo, (void*) new_socket);
	}
	if(tipo==TIPO_CPU){
		creacion = pthread_create(&newThread, &attr, &gestionarCPU, (void*) new_socket);
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
	enviarOKConContenido(socket, sizeof(int32_t), string_itoa(marco_size));
}

int makeHandshake(int socket){
	recibirResponse(socket);
	//TODO validar header
	response* tipo = recibirResponse(socket);
	return convertToInt32(tipo->contenido);
}

void manejarNuevasConexiones(){
	int new_socket = aceptarNuevaConexion(listener);
	int tipo = makeHandshake(new_socket);
	if(tipo == -1){
		printf("Hubo error en handshake");
		return;
	}
	if(tipo == TIPO_NUCLEO || tipo == TIPO_CPU){
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

	// int retardo = config_get_int_value(config, "RETARDO");

	initMemoriaPrincipal(config);
	initTLB(config);

	initSwap(config);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	crearHiloDeComponente(TIPO_SWAP, swap_socket);

	char* puerto_cpu_nucleo = config_get_string_value(config, "PUERTO_CPU_NUCLEO"); //puerto escucha de Nucleo y CPU
	listener = crear_puerto_escucha(puerto_cpu_nucleo);

	config_destroy(config);

	initUmcConsole();

	while(1){
		printf("Esperando conexiones...");
		manejarNuevasConexiones(); //Nucleo o CPU
	}

	close(listener); // bye!
	printf("Terminé felizmente");
	return EXIT_SUCCESS;
}
