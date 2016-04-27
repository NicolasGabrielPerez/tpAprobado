/*
 ============================================================================
 Name        : UCM.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

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

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int crear_socket_cliente(char* ip, char* port) { //devuelve un nuevo socket para conectarse al server especificado
	int sockfd; //aca se va a poner el socket obtenido mediante getaddrinfo

	struct addrinfo hints; //estructura conf info necesaria para getaddrinfo
	struct addrinfo *servinfo; //aca se va a poner la info del server
	struct addrinfo *p; //puntero usado para loopear los
	char s[INET6_ADDRSTRLEN]; //esto es para el nombre del server al que nos conectamos
	int rv; //valor que se usa para obtener especificacion de error, en caso de haberlo en getaddrinfo

	memset(&hints, 0, sizeof hints); //se asegura de 'limpiar' la memoria
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((getaddrinfo(ip, port, &hints, &servinfo)) != 0) {
	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) { //hubo error...
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	//se obtiene y se muestra el nombre del server
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
		s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	return sockfd;
}

int handshake(int sockfd){
	char buf[50];
	int numbytes; //lo uso para poner la cantidad de bytes recibidos
	puts("CPU: Voy a enviar algo...\n");
	if (send(sockfd,"Soy UMC    ", 11, 0) == -1) {
	  perror("send");
	}

	if ((numbytes = recv(sockfd, buf, 11, 0)) == -1) {
		perror("recv");
		exit(1);
	}

	printf("client: received '%s'\n",buf);
	printf("numbytes: '%d'\n",numbytes);
	buf[numbytes] = '\0';

	close(sockfd);
	puts("Swap: handshake finalizado felizmente\n");

	return 0;
}
int crear_puerto_escucha(char* port){
	int listener;
	int rv;
	struct addrinfo hints, *ai, *p;
	int yes = 1;
	// get us a socket and bind it
   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;
   if ((rv = getaddrinfo(NULL, port, &hints, &ai)) != 0) {
	   fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
	   exit(1);
   }

   for(p = ai; p != NULL; p = p->ai_next) {
	   listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
	   if (listener < 0) {
		   continue;
	   }

	   // lose the pesky "address already in use" error message
	   setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	   if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
		   close(listener);
		   continue;
	   }

	   break;
   }

   // if we got here, it means we didn't get bound
   if (p == NULL) {
	   fprintf(stderr, "selectserver: failed to bind\n");
	   exit(2);
   }

   freeaddrinfo(ai); // all done with this

   // listen
   if (listen(listener, 10) == -1) {
	   perror("listen");
	   exit(3);
   }

   return listener;
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

    fd_set master;    // master file descriptor list
   fd_set read_fds;  // temp file descriptor list for select()
   int fdmax;        // maximum file descriptor number
   int bytes_recibidos;
   int nucleo_y_cpu_listener, swap_listener;     // listening socket descriptor
   int newfd;        // newly accept()ed socket descriptor
   struct sockaddr_storage remoteaddr; // client address
   socklen_t addrlen;

   char buf[256];    // buffer for client data
   int nbytes;

   char remoteIP[INET6_ADDRSTRLEN];

   int i;

   FD_ZERO(&master);    // clear the master and temp sets
   FD_ZERO(&read_fds);

   nucleo_y_cpu_listener = crear_puerto_escucha(puerto_cpu_nucleo);
   swap_listener = crear_puerto_escucha(puerto_swap);

   printf("Creado listener: %d\n", nucleo_y_cpu_listener);

   // add the listener to the master set
   FD_SET(nucleo_y_cpu_listener, &master);
   FD_SET(swap_listener, &master);

   // keep track of the biggest file descriptor
   if(nucleo_y_cpu_listener> swap_listener){
   	   fdmax = nucleo_y_cpu_listener;
      }else{
   	   fdmax = swap_listener; // so far, it's this one
      }

   // main loop
   for(;;) {
	   puts("UMC: esperando conexiones...\n");
	   read_fds = master; // copy it
	   if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
		   perror("select");
		   exit(4);
	   }

	   // run through the existing connections looking for data to read
	   for(i = 0; i <= fdmax; i++) {
		   if (FD_ISSET(i, &read_fds)) { // we got one!!
			   if (i == nucleo_y_cpu_listener) {
				   puts("Cambió el listener de nucleo y cpu\n");
				   // handle new connections
				   addrlen = sizeof remoteaddr;
				   newfd = accept(nucleo_y_cpu_listener,
					   (struct sockaddr *)&remoteaddr,
					   &addrlen);

				   if (newfd == -1) {
					   perror("accept");
				   } else {
					   FD_SET(newfd, &master); // add to master set
						if (newfd > fdmax) {    // keep track of the max
							fdmax = newfd;
						}
						printf("selectserver: new connection from %s on "
							"socket %d\n",
							inet_ntop(remoteaddr.ss_family,
								get_in_addr((struct sockaddr*)&remoteaddr),
								remoteIP, INET6_ADDRSTRLEN),
							newfd);

						if ((bytes_recibidos = recv(newfd, buf, 11, 0)) == -1) {
						   perror("recv");
						   exit(1);
					   }

						printf("Se recibio: %s\nbytes_recibidos: %d.\n", buf, bytes_recibidos);

						if (send(newfd, "Soy la UMC ", 10, 0) == -1) {
							 perror("send");
						 }

						puts("Terminó el envío\n");
				   }
				   continue;
			   }

			   if(i == swap_listener){
				   puts("Cambió el listener de swap\n");
				   // handle new connections
				   addrlen = sizeof remoteaddr;
				   newfd = accept(swap_listener,
					   (struct sockaddr *)&remoteaddr,
					   &addrlen);

				   puts("Conexion aceptada");
				   if (newfd == -1) {
					   perror("accept");
				   } else {
					   FD_SET(newfd, &master); // add to master set
						if (newfd > fdmax) {    // keep track of the max
							fdmax = newfd;
						}
						printf("umc: new connection from %s on "
							"socket %d\n",
							inet_ntop(remoteaddr.ss_family,
								get_in_addr((struct sockaddr*)&remoteaddr),
								remoteIP, INET6_ADDRSTRLEN),
							newfd);

						printf("El fd es: %d", newfd);
						if ((bytes_recibidos = recv(newfd, buf, 8, 0)) == -1) {
						   perror("recv");
						   exit(1);
					   }

						printf("Se recibio: %s\nbytes_recibidos: %d.\n", buf, bytes_recibidos);

						if (send(newfd, "Soy UMC", 7, 0) == -1) {
							 perror("send");
						 }

						puts("Pasamos por el send...\n");
				   }
				   continue;
			   }

			   // handle data from a client
			   if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
				   // got error or connection closed by client
				   if (nbytes == 0) {
					   // connection closed
					   printf("selectserver: socket %d hung up\n", i);
				   } else {
					   perror("recv");
				   }
				   close(i); // bye!
				   FD_CLR(i, &master); // remove from master set
			   } else {
				   // we got some data from a client
				   puts("ucm: Voy a enviar algo...\n");
				   if (send(i, "Hola!", 5, 0) == -1) {
						 perror("send");
					 }
			   }


		   } // END got new incoming connection
	   } // END looping through file descriptors
	     //conexiones checkpoint
	           int PACKAGESIZE = 500;
	           int status = 1;
	           	char package[PACKAGESIZE];

	           	while(status){
	           			status = recv(socket_consola, (void*) package, PACKAGESIZE, 0);//cambiar socket al de cpu
	           			if (status) send(socket_cpu, package, strlen(package) + 1, 0);//cambiar socket al de swap
	           			if (status != 0) printf("%s", package);
	           	}


	           	close(clienteCPU);
	           	close(servidorSWAP);
   } // END for(;;)--and you thought it would never end!

   return 0;
}
