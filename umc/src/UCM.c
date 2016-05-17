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
#include <sockets/sockets.h>
#include <pthread.h>

#define TIPO_NUCLEO 1
#define TIPO_CPU 2
int listener;
int swap_socket;

void* gestionarNucleo(int socket){
	int bytes_recibidos;
	char buf[50];

	if ((bytes_recibidos = recv(socket, buf, 50, 0)) == -1) {
	   perror("recv");
	   exit(1);
	}

	return 0;
}

void* gestionarCPU(int socket){

	return 0;
}

int handshakeYDeterminarTipo(new_socket){
	int bytes_recibidos;
	char buf[50];
	int tipo;

	printf("El fd es: %d", new_socket);
	if ((bytes_recibidos = recv(new_socket, buf, 50, 0)) == -1) {
	   perror("recv");
	   exit(1);
	}

	printf("Se recibio: %s\nbytes_recibidos: %d.\n", buf, bytes_recibidos);



	if (send(new_socket, "Soy SWAP", 50, 0) == -1) {
		 perror("send");
	 }

	puts("Termino el handshake\n");

	tipo = *((int*)buf[0]);
	free(buf);
	return tipo;
}

void crearHiloDeComponente(int new_socket){
	int tipo = handshakeYDeterminarTipo(new_socket);
	pthread_t newThread;
	if(tipo==TIPO_NUCLEO){
		pthread_create(&newThread, NULL, gestionarNucleo(), (void*) new_socket);
	}
	if(tipo==TIPO_CPU){
		pthread_create(&newThread, NULL, gestionarCPU(), (void*) new_socket);
	}

}

void aceptarConexiones(){
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
	} else {

	printf("umc: new connection from %s on "
		"socket %d\n",
		inet_ntop(remoteaddr.ss_family,
			get_in_addr((struct sockaddr*)&remoteaddr),
			remoteIP, INET6_ADDRSTRLEN),
			listener);

	crearHiloDeComponente(new_socket);

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
//    int cantdiad_de_marcos = config_get_int_value(config, "MARCOS");
//    int marco_size = config_get_int_value(config, "MARCO_SIZE");
//    int ip_marcos_x_proc = config_get_int_value(config, "MARCOS_X_PROC");
//    int cantidad_entradas_tlb = config_get_int_value(config, "ENTRADAS_TLB");
//    int retardo = config_get_int_value(config, "RETARDO");

    printf("Config: PUERTO_CPU_NUCLEO=%s\n", puerto_cpu_nucleo);
    printf("Config: IP_SWAP=%s\n", ip_swap);
    printf("Config: PUERTO_SWAP=%s\n", puerto_swap);

   listener = crear_puerto_escucha(puerto_cpu_nucleo);
   swap_socket = crear_socket_cliente("utnso40", puerto_swap);

   handshake(swap_socket, "soy umc");

   printf("Creado listener: %d\n", listener);

   while(1){
		puts("Esperando conexiones...");
		aceptarConexiones(); //crear un hilo para la nueva conexion
   }

    close(listener); // bye!

    puts("Terminé felizmente");
	return EXIT_SUCCESS;
}
