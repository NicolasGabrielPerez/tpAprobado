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
#include "../../push-library/sockets/sockets.h"
#include <pthread.h>

#include "umc-interfaz.h"

#define TIPO_NUCLEO 1
#define TIPO_CPU 2
int listener;
int swap_socket;
t_umc_main_memory memoria;
t_umc_cache_tlb cache_tlb;

void *gestionarNucleo(void* socket){
	int bytes_recibidos;
	char buf[50];

	if ((bytes_recibidos = recv((int)socket, buf, 50, 0)) == -1) {
	   perror("recv");
	   exit(1);
	}

	return 0;
}

void *gestionarCPU(void* socket){

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
		creacion = pthread_create(&newThread, NULL, &gestionarNucleo, (void*) new_socket);
	}
	if(tipo==TIPO_CPU){
		creacion = pthread_create(&newThread, NULL, &gestionarCPU, (void*) new_socket);
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

void gestionarNuevasConexiones(){
	int new_socket = aceptarNuevaConexion();
	int tipo = makeHandshake(new_socket);
	if(tipo == -1){
		puts("Hubo error en handshake");
		return;
	}
	if(crearHiloDeComponente(tipo, new_socket) == -1){
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

int main(void) {
	t_config* config = config_create("umc.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}
    char* puerto_cpu_nucleo = config_get_string_value(config, "PUERTO_CPU_NUCLEO"); //puerto escucha de Nucleo y CPU
    char* ip_swap = config_get_string_value(config, "IP_SWAP");
    char* puerto_swap = config_get_string_value(config, "PUERTO_SWAP"); //puerto escucha de swap
    int cantidad_de_marcos = config_get_int_value(config, "MARCOS");
    int marco_size = config_get_int_value(config, "MARCO_SIZE");
//    int ip_marcos_x_proc = config_get_int_value(config, "MARCOS_X_PROC");
    int cantidad_entradas_tlb = config_get_int_value(config, "ENTRADAS_TLB");
//    int retardo = config_get_int_value(config, "RETARDO");

    printf("Config: PUERTO_CPU_NUCLEO=%s\n", puerto_cpu_nucleo);
    printf("Config: IP_SWAP=%s\n", ip_swap);
    printf("Config: PUERTO_SWAP=%s\n", puerto_swap);

    initiMemoriaPrincipal(cantidad_de_marcos, marco_size);
    initCache(cantidad_entradas_tlb, marco_size);

   listener = crear_puerto_escucha(puerto_cpu_nucleo);
   swap_socket = crear_socket_cliente("utnso40", puerto_swap);

   handshake(swap_socket, "soy umc");

   printf("Creado listener: %d\n", listener);

   while(1){
		puts("Esperando conexiones...");
		gestionarNuevasConexiones(); //crear un hilo para la nueva conexion
   }

    close(listener); // bye!

    puts("Terminé felizmente");
	return EXIT_SUCCESS;
}
