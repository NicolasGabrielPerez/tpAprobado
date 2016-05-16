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
   int nucleo_y_cpu_listener, swap_socket;     // listening socket descriptor
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
   swap_socket = crear_socket_cliente("utnso40", puerto_swap);

   handshake(swap_socket, "soy umc");

   printf("Creado listener: %d\n", nucleo_y_cpu_listener);

   // add the listener to the master set
   FD_SET(nucleo_y_cpu_listener, &master);

   // keep track of the biggest file descriptor
   fdmax = nucleo_y_cpu_listener;

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
				   puts("Se recibe data de un cliente que ya existe\n");
				   printf("Se recibieron %d bytes\n", nbytes);
				   printf("Se recibio: %s\n", buf);
				   puts("Le mando lo mismo SWAP\n");
				   if ( send(swap_socket, buf, sizeof(buf), 0) == -1) { //envio lo mismo que me acaba de llegar => misma cant de bytes a enviar
						 perror("send");
				   };
			   }


		   } // END got new incoming connection
	   } // END looping through file descriptors
   } // END for(;;)--and you thought it would never end!

   return 0;
}
