#include <pthread.h>
#include "umc-structs.h"
#include "swap-interfaz.h"
#include "cpu-interfaz.h"
#include "nucleo-interfaz.h"
#include "tlb.h"

//TODO en caso de Fallo, enviar un int32 adicional con el codigo de error

pthread_attr_t attr;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//TODO poder usar estas variables desde la push-library
int32_t HEADER_CAMBIO_PROCESO_ACTIVO = 700;
int32_t TIPO_NUCLEO = 2000;
int32_t TIPO_CPU = 5000;
int32_t TIPO_UMC = 3000;
int32_t TIPO_SWAP = 4000;

int listener;

int* convertToInt32(char* buffer){
	int32_t* number = malloc(sizeof(int32_t));
	memcpy(number, buffer, sizeof(int32_t));
	return number;
}

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

int aceptarNuevaConexion(){
	int new_socket;
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;
	char remoteIP[INET6_ADDRSTRLEN];
	// handle new connections
	addrlen = sizeof remoteaddr;
	new_socket = accept(listener,
	(struct sockaddr *)&remoteaddr,
	&addrlen);
	puts("Conexion aceptada");
	if (new_socket == -1) {
	perror("accept");
	}
	printf("umc: new connection from %s on "
	"socket %d\n",
	inet_ntop(remoteaddr.ss_family,
	get_in_addr((struct sockaddr*)&remoteaddr),
	remoteIP, INET6_ADDRSTRLEN),
	listener);
	return new_socket;
}

int makeHandshake(int socket){
	char* header = malloc(HEADER_SIZE);
	if (recv((int)socket, header, HEADER_SIZE, 0) == -1) {
		perror("recv");
		exit(1);
	}
	char* type;
	if (recv((int)socket, header, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}
	int32_t* iType = convertToInt32(type);
	if(! (type==TIPO_NUCLEO || type==TIPO_CPU)){
		if (send(socket, &RESPUESTA_FAIL, sizeof(int32_t), 0) == -1) {
			perror("send");
			exit(1);
		};
		return EXIT_FAILURE;
	}

	if (send(socket, &RESPUESTA_OK, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	};
	if (send(socket, &TIPO_UMC, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	};
	return iType;
}

void manejarNuevasConexiones(){
	int new_socket = aceptarNuevaConexion();
	int tipo = makeHandshake(new_socket);
	if(tipo == -1){
		puts("Hubo error en handshake");
		return;
	}
	int creacion = crearHiloDeComponente(tipo, new_socket);
	if(creacion == -1){
		puts("Error al crear hilo\n");
	}
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

	//TODO
	// Crear hilo para consola

	while(1){
		puts("Esperando conexiones...");
		manejarNuevasConexiones(); //Nucleo o CPU
	}

	close(listener); // bye!
	puts("Terminé felizmente");
	return EXIT_SUCCESS;
}
