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
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <comunicacion/sockets.h>

void imprimirClaveValor(char** claves, char** valores){
	char* clave;
	char* valor;
	int i;
	for(i = 0; claves[i]!=NULL; i++){
		clave = *claves;
		valor = *valores;
		printf("Config: %s,%s\n", clave, valor);
	}
}

int main(void) {
		t_config* config = config_create("nucleo.config");
		char* puerto_prog = config_get_string_value(config, "PUERTO_PROG"); //puerto escucha de Consola
	    char* puerto_cpu = config_get_string_value(config, "PUERTO_CPU"); //puerto escucha de CPU
	    char* puerto_umc = config_get_string_value(config, "PUERTO_UMC"); //puerto de UMC
	    int package_size = config_get_int_value(config, "PACKAGESIZE");
	    int backlog = config_get_int_value(config, "BACKLOG");
	    int quantum = config_get_int_value(config, "QUANTUM");
	    int quantum_sleep = config_get_int_value(config, "QUANTUM_SLEEP");

	    printf("Config: PUERTO_PROG=%s\n", puerto_prog);
		printf("Config: PUERTO_CPU=%s\n", puerto_cpu);
		printf("Config: PUERTO_UMC=%s\n", puerto_umc);
		printf("Config: PACKAGESIZE=%d\n", package_size);
		printf("Config: BACKLOG=%d\n", backlog);
		printf("Config: QUANTUM=%d\n", quantum);
		printf("Config: QUANTUM_SLEEP=%d\n", quantum_sleep);

		char** io_ids = config_get_array_value(config, "IO_ID");
		char** io_sleep_times = config_get_array_value(config, "IO_SLEEP");

		imprimirClaveValor(io_ids, io_sleep_times);

		//para obtener un value, usas (char *)dictionary_get(x_dictionary,clave)
		//OJO, el t_dictionary solo funciona de strings a strings,
		//o sea que para sacar un int, primero lo sacas como char*
		//despues lo podes convertir a int con la funcion atoi()

	    //t_dictionary* io_dictionary = create_dictionary_from_strings(config, io_ids, io_sleep_times); //lo creo por las dudas

	    //char** semaforos_ids = config_get_array_value(config, "SEM_ID");
	    //char** semaforos_init_values = config_get_array_value(config, "SEM_INIT");

	    //char** shared_values = config_get_array_value(config, "SHARED_VARS");

	    int socket_umc = crear_socket_cliente("utnso40", puerto_umc);
	    handshake(socket_umc, "Prueba");

	    fd_set master;    // master file descriptor list
       fd_set read_fds;  // temp file descriptor list for select()

       int fdmax;        // maximum file descriptor number
       int bytes_recibidos;
       int consola_listener, cpu_listener;     // listening socket descriptor
       int newfd;        // newly accept()ed socket descriptor
       struct sockaddr_storage remoteaddr; // client address
       socklen_t addrlen;

       char buf[50];    // buffer for client data
       int nbytes;

       char remoteIP[INET6_ADDRSTRLEN];

       int yes=1;        // for setsockopt() SO_REUSEADDR, below
       int i, rv;



       FD_ZERO(&master);    // clear the master and temp sets
       FD_ZERO(&read_fds);

       consola_listener = crear_puerto_escucha(puerto_prog);

       printf("Creado listener de Consola: %d\n", consola_listener);

       cpu_listener = crear_puerto_escucha(puerto_cpu);
       int cpu_socket;
	   printf("Creado listener: %d\n", cpu_listener);

       // agrego listener de consola
       FD_SET(consola_listener, &master);

       // agrego listener de consola
       FD_SET(cpu_listener, &master);

       // keep track of the biggest file descriptor
       if(consola_listener> cpu_listener){
    	   fdmax = consola_listener;
       }else{
    	   fdmax = cpu_listener; // so far, it's this one
       }

       t_list* consola_sockets = list_create();
       t_list* cpu_sockets = list_create();
       int listSize;

       // main loop
       for(;;) {
    	   puts("NUCLEO: Esperando conexiones...\n");
           read_fds = master; // copy it
           if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
               perror("select");
               exit(4);
           }

           // run through the existing connections looking for data to read
           for(i = 0; i <= fdmax; i++) {
               if (FD_ISSET(i, &read_fds)) { // we got one!!
                   if (i == consola_listener) {
                	   puts("Cambió el listener de consola\n");
                	  	// handle new connections
                	  	addrlen = sizeof remoteaddr;
                	  	newfd = accept(consola_listener,
                	  	(struct sockaddr *)&remoteaddr,
                	  	&addrlen);

                	  	if (newfd == -1) {
                	  	perror("accept");
                	  	} else {
                	  	FD_SET(newfd, &master); // add to master set
                	  	if (newfd > fdmax) {    // keep track of the max
                	  	   fdmax = newfd;
                	  	}
                	  	printf("nucleo: new connection from %s on "
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

                	  	if (send(newfd, "Soy NUCLEO", 10, 0) == -1) {
                	  		 perror("send");
                	  	 }

                	  	puts("Terminó el handshake\n");
                	  	list_add(consola_sockets, i);

                	  	puts("Se agregó socket de consola\n");
                       }

                	  	//------------------Hasta acá es sólo handshake------------------
                	  	continue;
                   }

                   if (i == cpu_listener) {
					   puts("Cambió el listener de cpu\n");
						// handle new connections
						addrlen = sizeof remoteaddr;
						newfd = accept(cpu_listener,
						(struct sockaddr *)&remoteaddr,
						&addrlen);

						if (newfd == -1) {
						perror("accept");
						} else {
							FD_SET(newfd, &master); // add to master set
							if (newfd > fdmax) {    // keep track of the max
							   fdmax = newfd;
							}
							printf("nucleo: new connection from %s on "
							   "socket %d\n",
							   inet_ntop(remoteaddr.ss_family,
								   get_in_addr((struct sockaddr*)&remoteaddr),
								   remoteIP, INET6_ADDRSTRLEN),
							   newfd);

							printf("El fd es: %d", newfd);
							if ((bytes_recibidos = recv(newfd, buf, sizeof(buf), 0)) == -1) {
							   perror("recv");
							   exit(1);
							}

							printf("Se recibio: %s\nbytes_recibidos: %d.\n", buf, bytes_recibidos);

							if (send(newfd, "Soy NUCLEO", 11, 0) == -1) {
								 perror("send");
							 }

							puts("Terminó el handshake\n");
							//list_add(cpu_sockets, newfd);
							cpu_socket = newfd;
							printf("Socket de cpu agregado: %d\n", cpu_socket);
						 }

						continue;
				}

				   // handle data from a client
				   if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
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
					   //se recibió mensaje
					   puts("Se recibe data de un cliente que ya existe\n");
					   printf("Se recibieron %d bytes\n", nbytes);
					   printf("Se recibió%s\n", buf);
					   //Hago de cuenta que se validó que lo que acaba de llegar es de la consola
					   // Y le quiero mandar a cpu

					   printf("Socket de cpu:%d\n", cpu_socket);
					   printf("Voy a mandar: %s\n", buf);
						if (send(cpu_socket, buf, sizeof(buf) , 0) == -1) {
							 perror("send");
						 }
//					   int j;
//					   for(j=0; j<list_size(cpu_sockets); j++){
////						   printf("Le mando al socket %d (cpu)\n", list_get(cpu_sockets, j));
////						   if (send(list_get(cpu_sockets, j), buf, sizeof(buf), 0) == -1) { //envio lo mismo que me acaba de llegar => misma cant de bytes a enviar
////								 perror("send");
////						   };
//
//					   }


				  } // END handle data from client

               } // END got new incoming connection
           } // END looping through file descriptors
       } // END for(;;)--and you thought it would never end!

       return 0;
}
