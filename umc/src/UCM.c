#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//#include "../../../push-library/sockets/sockets.h"
#include <pthread.h>
#include <sockets/sockets.h>
#include "umc-interfaz.h"

#define TIPO_NUCLEO 1
#define TIPO_CPU 2
#define TIPO_SWAP 3

#define PEDIDO_LECTURA 1
int NUCLEO_HEADER_SIZE = sizeof(int32_t); //tipo de pedido: init o finalizar programa
int PEDIDO_INIT_PROGRAMA_SIZE = sizeof(int32_t)*3;
int RESPUESTA_OK_FAIL_SIZE = sizeof(int32_t);
int32_t PEDIDO_INIT_PROGRAMA = 1;
int32_t PEDIDO_FINALIZAR_PROGRAMA = 6;
int32_t OK = 1;
int32_t FAIL = 2;
char* mensajeOKEY = "OKEY";
char* mensajeFAIL = "FAIL";
int listener;
int swap_socket;
t_umc_main_memory memoria;
t_umc_cache_tlb cache_tlb;
pthread_attr_t attr;

pthread_mutex_t swap = PTHREAD_MUTEX_INITIALIZER;

char* serializarPedidoAlmacenamientoSwap(char* pid, char* nroPagina, char* offset, char* tamanio, char* contenido){
	int pidSize, nroPaginaSize, offsetSize, tamanioSize;
	pidSize = nroPaginaSize = offsetSize = tamanioSize = sizeof(int32_t);
	int contenidoSize = strlen(sizeof(contenido));
	char* pedido = malloc(pidSize+nroPaginaSize+offsetSize+tamanioSize+contenidoSize);
	char* siguiente = pedido;
	memcpy(siguiente, pid, pidSize);
	siguiente += pidSize;
	memcpy(siguiente, nroPagina, nroPaginaSize);
	siguiente += nroPaginaSize;
	memcpy(siguiente, offset, offsetSize);
	siguiente += offsetSize;
	memcpy(siguiente, tamanio, tamanioSize);
	siguiente += tamanioSize;
	memcpy(siguiente, contenido, contenidoSize);
	return pedido;
}

int solicitarAlmacenamientoASwap(pid, nroPagina, offset, tamanio, contenido){

	char* pedidoSerializado = serializarPedidoAlmacenamientoSwap(pid, nroPagina, offset, tamanio, contenido);
	char* respuestaDeSwap = malloc(RESPUESTA_OK_FAIL_SIZE);
	pthread_mutex_lock(swap);
		if (send(swap_socket, pedidoSerializado,strlen(pedidoSerializado), 0) == -1){
			perror("send");
			pthread_mutex_unlock(swap);
			return FAIL;
		}
		if (recv(swap_socket, respuestaDeSwap, RESPUESTA_OK_FAIL_SIZE, 0) == -1) {
		   perror("recv");
		   pthread_mutex_unlock(swap);
		   return FAIL;
		};
	pthread_mutex_unlock(swap);
	return OK;
}

int derivarPaginasASwap(int pid, int cantidadDePaginas, int socket_solicitante){
	int i;
	int respuestaDeSwap = OK;
	int size_of_int32 = sizeo(int32_t);
	char* nroPagina = malloc(size_of_int32);
	char* offset = malloc(size_of_int32);
	char* tamanio = malloc(size_of_int32);
	char* contenido = malloc(size_of_int32);
	for(i=0;i<cantidadDePaginas;i++){
		if (recv(socket_solicitante, nroPagina, malloc(size_of_int32), 0) == -1) {
		   perror("recv");
		   respuestaDeSwap = FAIL;
		   break;
		};
		if (recv(socket_solicitante, offset, malloc(size_of_int32), 0) == -1) {
		   perror("recv");
		   respuestaDeSwap = FAIL;
		   break;
		};
		if (recv(socket_solicitante, tamanio, malloc(size_of_int32), 0) == -1) {
		   perror("recv");
		   respuestaDeSwap = FAIL;
		   break;
		};
		if (recv(socket_solicitante, contenido, string_itoa(tamanio), 0) == -1) {
		   perror("recv");
		   respuestaDeSwap = FAIL;
		   break;
		};
		respuestaDeSwap = solicitarAlmacenamientoASwap(pid, nroPagina, offset, tamanio, contenido);
		if(respuestaDeSwap == FAIL){
			break;
		}
	}
	return respuestaDeSwap;
}

char* initProgramaSwap(int32_t pid, int32_t cantidadDePaginas){
	char* pedido = serializarPedidoInit(pid, cantidadDePaginas);
	if(strlen(pedido) != PEDIDO_INIT_PROGRAMA_SIZE){
		return 0;
	}
	char* respuestaDeSwap = malloc(RESPUESTA_OK_FAIL_SIZE);
	pthread_mutex_lock(swap);
	if (send(swap_socket, pedido, PEDIDO_INIT_PROGRAMA_SIZE, 0) == -1) {
	 perror("send");
	 pthread_mutex_unlock(swap);
	 exit(1);
	}
	if (recv((int)socket, pedido, respuestaDeSwap, 0) == -1) {
	   perror("recv");
	   pthread_mutex_unlock(swap);
	   exit(1);
	}
	pthread_mutex_unlock(swap);
	return respuestaDeSwap;
}

char* finalizarPrograma(char* pedido){
	int32_t pid;
	int32_t cantidadDePaginas;

	if (recv((int)socket, pid, sizeof(int32_t), 0) == -1) {
	   perror("recv");
	   exit(1);
	}

}

int initPrograma(int socket_nucleo){
	int32_t pid;
	int32_t cantidadDePaginas;

	if (recv((int)socket, pid, sizeof(int32_t), 0) == -1) {
	   perror("recv");
	   exit(1);
	}

	if (recv((int)socket, cantidadDePaginas, sizeof(int32_t), 0) == -1) {
	   perror("recv");
	   exit(1);
	}

	printf("Inicializando programa con pid: %d y cantidad de paginas: %d\n", pid, cantidadDePaginas);

	char* respuestaDeSwap = initProgramaSwap(pid, cantidadDePaginas);

	int32_t iRespuestaDeSwap;
	memcpy(&iRespuestaDeSwap, respuestaDeSwap, sizeof(int32_t));
	if(iRespuestaDeSwap == OK){
		 derivarPaginasASwap(pid, cantidadDePaginas, socket_nucleo);
		 initProgramaMemoriaPrincipal(pid, cantidadDePaginas);
	}
	if(iRespuestaDeSwap == FAIL){
		printf("No se pudo incializar pid %d\n", pid);
		return FAIL;
	}

	return 0;
}

int getPedidoType(char*){
	return 0;
}

char* leerPaginas(char* pedido){
	int estaEnMemoriaPrincipal = 0;
	if(estaEnMemoriaPrincipal){

	} else{
		solicitarLecturaASwap(pedido);
	}
	return 0;
}

char* delegarPedidoCPU(char* pedido){
	char* respuesta;
	int pedido_type = getPedidoType(pedido);

	if(pedido_type == PEDIDO_LECTURA){
		respuesta = leerPaginas(pedido);
	}

	return respuesta;
}

void *gestionarNucleo(void* socket_nucleo){

	printf("Creado hilo de gestión de NUCLEO\n");
	printf("De socket: %d\n", (int)socket);

	char pedido[NUCLEO_HEADER_SIZE];
	int32_t iPedido;

	while(1){
		if (recv((int)socket, pedido, NUCLEO_HEADER_SIZE, 0) == -1) {
		   perror("recv");
		   exit(1);
		}

		memcpy(&iPedido, pedido, sizeof(int32_t));

		if(iPedido==PEDIDO_INIT_PROGRAMA){
			initPrograma(socket_nucleo);
		}
		else if(iPedido==PEDIDO_FINALIZAR_PROGRAMA){
			finalizarPrograma(socket_nucleo);
		}
		else {
			send(socket_nucleo, mensajeFAIL, strlen(mensajeFAIL), NULL);
		}
	}

	return 0;
}

void *gestionarCPU(void* socket){

	printf("Creado hilo de gestión de CPU\n");
	printf("De socket: %d\n", (int)socket);

	int bytes_recibidos;
	char pedido[50];
	char* respuesta;

	while(1){
		if ((bytes_recibidos = recv((int)socket, pedido, 50, 0)) == -1) {
		   perror("recv");
		   exit(1);
		}

		respuesta = procesarPedido(pedido);

		if (send(socket, respuesta, 50, 0) == -1) {
			 perror("send");
			 exit(1);
		 }
	}

	return 0;
}

int makeHandshake(int new_socket){
	int tipo = recibir_handshake(new_socket);

	if(tipo==-1){
		puts("Hubo error recibiendo handshake\n");
		return -1;
	}

	if(devolver_handshake(new_socket, tipo) == -1){
		puts("Hubo error en el handshake");
		return -1;
	}

	return tipo;

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

void gestionarNuevasConexionesCPU(){
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

void initiMemoriaPrincipal(int marcos, int marco_size){
	char memory_block[marcos*marco_size];
	memoria.memory = memory_block;
}

void initCache(int cantidad_entradas_tlb, int marco_size){
	if(cantidad_entradas_tlb==0) return;
}

void initSwap(int puerto_swap, int ip_swap){
   swap_socket = crear_socket_cliente("utnso40", puerto_swap);
   handshake(swap_socket, "soy umc");
}

void initNucleo(){
	int socket = aceptarNuevaConexion(listener);
    char* respuesta = handshake(socket, "soy umc");
    //TODO chequear que el componente sea Nucleo
    crearHiloDeComponente(TIPO_NUCLEO, socket);
}

int main(void) {
	t_config* config = config_create("umc.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}
    char* puerto_cpu_nucleo = config_get_string_value(config, "PUERTO_CPU_NUCLEO"); //puerto escucha de Nucleo y CPU
    char* ip_swap = config_get_string_value(config, "IP_SWAP");
    char* puerto_swap = config_get_string_value(config, "PUERTO_SWAP"); //puerto de swap
    //int cantidad_de_marcos = config_get_int_value(config, "MARCOS");
    //int marco_size = config_get_int_value(config, "MARCO_SIZE");
//    int ip_marcos_x_proc = config_get_int_value(config, "MARCOS_X_PROC");
    //int cantidad_entradas_tlb = config_get_int_value(config, "ENTRADAS_TLB");
//    int retardo = config_get_int_value(config, "RETARDO");

    printf("Config: PUERTO_CPU_NUCLEO=%s\n", puerto_cpu_nucleo);
    printf("Config: IP_SWAP=%s\n", ip_swap);
    printf("Config: PUERTO_SWAP=%s\n", puerto_swap);

    //initiMemoriaPrincipal(cantidad_de_marcos, marco_size);
    //initCache(cantidad_entradas_tlb, marco_size);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

   listener = crear_puerto_escucha(puerto_cpu_nucleo);
   printf("Creado listener: %d\n", listener);

   initSwap(puerto_swap, ip_swap);
   initNucleo();

   //TODO
   // Crear hilo para consola

   while(1){
		puts("Esperando conexiones...");
		gestionarNuevasConexionesCPU(); //crear un hilo para la nueva conexion
   }

    close(listener); // bye!

    puts("Terminé felizmente");
	return EXIT_SUCCESS;
}
