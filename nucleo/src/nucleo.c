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
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
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
}

int main(void) {
		t_config* config = config_create("nucleo.config");
		char* puerto_prog = config_get_string_value(config, "PUERTO_PROG");
	    char* puerto_cpu = config_get_string_value(config, "PUERTO_CPU");
	    int package_size = config_get_int_value(config, "PACKAGESIZE");
	    int backlog = config_get_int_value(config, "BACKLOG");
	    int quantum = config_get_int_value(config, "QUANTUM");
	    int quantum_sleep = config_get_int_value(config, "QUANTUM_SLEEP");

		char** io_ids = config_get_array_value(config, "IO_ID");
		char** io_sleep_times = config_get_array_value(config, "IO_SLEEP");

		//para obtener un value, usas (char *)dictionary_get(x_dictionary,clave)
		//OJO, el t_dictionary solo funciona de strings a strings,
		//o sea que para sacar un int, primero lo sacas como char*
		//despues lo podes convertir a int con la funcion atoi()

	    //t_dictionary* io_dictionary = create_dictionary_from_strings(config, io_ids, io_sleep_times); //lo creo por las dudas

	    char** semaforos_ids = config_get_array_value(config, "SEM_ID");
	    char** semaforos_init_values = config_get_array_value(config, "SEM_INIT");

	    char** shared_values = config_get_array_value(config, "SHARED_VARS");

	    fd_set master;    // master file descriptor list
       fd_set read_fds;  // temp file descriptor list for select()
       fd_set cpus;
       fd_set consolas;

       int fdmax;        // maximum file descriptor number
       int bytes_recibidos;
       int listener;     // listening socket descriptor
       int newfd;        // newly accept()ed socket descriptor
       struct sockaddr_storage remoteaddr; // client address
       socklen_t addrlen;

       char buf[256];    // buffer for client data
       int nbytes;

       char remoteIP[INET6_ADDRSTRLEN];

       int yes=1;        // for setsockopt() SO_REUSEADDR, below
       int i, rv;

       FD_ZERO(&master);    // clear the master and temp sets
       FD_ZERO(&read_fds);

       listener = crear_puerto_escucha("8989");

       // add the listener to the master set
       FD_SET(listener, &master);

       // keep track of the biggest file descriptor
       fdmax = listener; // so far, it's this one

       // main loop
       for(;;) {
    	   puts("Comienzo del main loop\n");
           read_fds = master; // copy it
           if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
               perror("select");
               exit(4);
           }

           // run through the existing connections looking for data to read
           for(i = 0; i <= fdmax; i++) {
               if (FD_ISSET(i, &read_fds)) { // we got one!!
                   if (i == listener) {
                	   puts("Cambió el listener\n");
                	   // handle new connections
                       addrlen = sizeof remoteaddr;
                       newfd = accept(listener,
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

                           printf("El fd es: %d", newfd);
                           if ((bytes_recibidos = recv(newfd, buf, 5, 0)) == -1) {
							   perror("recv");
							   exit(1);
						   }

                           printf("Se recibio: %s\nbytes_recibidos: %d.\n", buf, bytes_recibidos);

                           if (send(newfd, "Nucl!", 5, 0) == -1) {
								 perror("send");
							 }

                           puts("Pasamos por el send...");

                       }
                   } else {
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
                    	   //se recibió mensaje
                           // we got some data from a client
                    	   puts("Núcleo: Voy a enviar algo...\n");
                    	   if (send(i, "Hola!", 5, 0) == -1) {
								 perror("send");
							 }
                       }
                   } // END handle data from client
               } // END got new incoming connection
           } // END looping through file descriptors
       } // END for(;;)--and you thought it would never end!

       return 0;
}
