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
#include <sockets/sockets.h>
#include <sockets/pcb.h>
#include <sockets/serialization.h>
#include <parser/metadata_program.h>

//TODO: Terminar de imprimir los valores de metadata del programa

//TODO: Serializar estructura PCB
//TODO: Handshake => recibo tamaño de página
//TODO: Envíar mensaje de programInit a UMC serializando PId, cant de páginas de memora requeridas, código fuente

#define INITIAL_SIZE 4
#define PRIMITIVE_SEPARATOR "$!"
#define CODEINDEX_SEPARATOR "$#"

void imprimirClaveValor(char** claves, char** valores) {
	char* clave;
	char* valor;
	int i;
	for (i = 0; claves[i] != NULL; i++) {
		clave = *claves;
		valor = *valores;
		printf("Config: %s,%s\n", clave, valor);
	}
}

void print_program_metadata(t_metadata_program *programMetadata) {
	puts("++++++++++++Program Metadada++++++++++++ \n");
	printf("t_size: %d \n", programMetadata->instrucciones_size);
	printf("Instrucción inicio: %d \n", programMetadata->instruccion_inicio);

	int j = 0;
	//int k = 0;
	printf("Cantidad de etiquetas: %d \n",
			programMetadata->cantidad_de_etiquetas);
	printf("Cantidad de funciones: %d \n",
			programMetadata->cantidad_de_funciones);

	puts("------------------Etiquetas------------------ \n");
	//for(k = 0 ; k < programMetadata->cantidad_de_funciones ; k ++){
	//	printf("La etiqueta %d es: %s \n", k, programMetadata->etiquetas[k]);
	//}
	printf("Etiquetas: %s \n", programMetadata->etiquetas);
	printf("Tamaño de etiquetas: %d \n", programMetadata->etiquetas_size);

	//t_puntero_instruccion tpi =  metadata_buscar_etiqueta("triple", programMetadata->etiquetas, programMetadata->etiquetas_size);
	//printf("Posición etiqueta: %d \n", tpi);

	puts("------------------Instrucciones------------------");
	printf("Cantidad de instrucciones: %d \n \n",
			programMetadata->instrucciones_size);
	for (j = 0; j < programMetadata->instrucciones_size; j++) {
		printf("**Comienzo instrucción %d: %d \n", j,
				programMetadata->instrucciones_serializado[j].start);
		printf("****Offset %d: %d \n", j,
				programMetadata->instrucciones_serializado[j].offset);
	}

	//char **prueba = string_split(programMetadata->etiquetas, "\0");
	//printf(">>>>>>>>>>>>>>>>>>PRobando %s, %d, %d, %d \n", prueba[0], prueba[1],prueba[2],prueba[3]);

}

void print_program_instruction_index(t_intructions *instructionIndex, t_size instructionsCount){
	printf("---------- Índice de instrucciones ---------- \n");

	int i;
	for (i = 0 ; i < instructionsCount ; i++){
		printf("Índice instrucción %d: \n", i);
		printf("---- Start: %d \n", instructionIndex[i].start);
		printf("---- Offset: %d \n", instructionIndex[i].offset);
	}

	printf("---------- Fín de índice ---------- \n");
}

void print_program_tags(char* tags, int32_t tagsCount, t_size tags_size){
	char** tagsArray = string_split(tags, "\0");
	int i;

	printf("---------- Etiquetas ----------\n");
	for (i = 0 ; i < tagsCount ; i++){
		printf("Tag n° %d: %s \n", i, tagsArray[i]);
	}
	printf("---------- Fin Etiquetas ----------\n");
}

//TODO: Verificar si está bien esto o si el tag index es el campo etiquetas de program metadata
void set_pcb_tag_index(PCB* pcb, t_metadata_program* programMetadata){
	pcb->tagIndex = dictionary_create();

	char* objetivo = "doble";

	t_puntero_instruccion punteroInstruccion = metadata_buscar_etiqueta(objetivo, programMetadata->etiquetas, programMetadata->etiquetas_size);
	printf("Puntero a instrucción doble: %d \n", punteroInstruccion);
	if(!dictionary_has_key(pcb->tagIndex, objetivo)){

	}

}



//http://stackoverflow.com/questions/6002528/c-serialization-techniques



//TODO: Serializar índice de etiquetas

//TODO: Serializar estructura de stack

//Serializa la estructura de código de programa entera
void serialize_pcb(PCB *pcb, Buffer *output) {
    serialize_int(pcb->processId, output);
    serialize_int(pcb->programCounter, output);
    serialize_int(pcb->codePagesCount, output);
    serialize_int(pcb->stackIndex, output);
}

void deserialize_string(Buffer *buffer){
	char** vector = string_split((char*)buffer->data, PRIMITIVE_SEPARATOR);

	int i;

	for(i = 0 ; i < 2 ; i++){
		printf("Vector en %d contiene: %s \n", i, vector[i]);
	}
}

void initialize_pcb_test(PCB *pcb){
	pcb->processId = 493;
	pcb->programCounter = 0;
	pcb->codePagesCount = 25;
	pcb->stackIndex = 10;
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

	char *programa1 = "function triple\n"
			"variables f\n"
			"f = $0 + $0 + $0\n"
			"return f\n"
			"end\n"

			"begin \n"
			"variables a,g \n"
			"a = 1\n"
			"g <- doble a\n"
			"print g\n"
			"end\n"

			"function doble\n"
			"variables f\n"
			"f = $0 + $0\n"
			"return f\n"
			"end";

	t_metadata_program *programMetadata = malloc(sizeof(t_metadata_program));

	programMetadata = metadata_desde_literal(programa1);

	print_program_metadata(programMetadata);

	struct Buffer *bufferTest = new_buffer();
	serialize_codeIndex(programMetadata->instrucciones_serializado, programMetadata->instrucciones_size, bufferTest);

	printf("CodeIndex serializado: %s \n", bufferTest->data);

	t_intructions* programInstructionIndex = deserialize_codeIndex(bufferTest->data, programMetadata->instrucciones_size);

	print_program_instruction_index(programInstructionIndex, programMetadata->instrucciones_size);

	//-------------- PRUEBA DE CREACIÓN DE TAG INDEX --------------
	PCB* pcb = malloc(sizeof(PCB));

	set_pcb_tag_index(pcb, programMetadata);
	//-------------- PRUEBA DE CREACIÓN DE TAG INDEX --------------

	print_program_tags(programMetadata->etiquetas, programMetadata->cantidad_de_etiquetas, programMetadata->etiquetas_size);

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

	int yes = 1;        // for setsockopt() SO_REUSEADDR, below
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
	if (consola_listener > cpu_listener) {
		fdmax = consola_listener;
	} else {
		fdmax = cpu_listener; // so far, it's this one
	}

	t_list* consola_sockets = list_create();
	t_list* cpu_sockets = list_create();
	int listSize;

	// main loop
	for (;;) {
		puts("NUCLEO: Esperando conexiones...\n");
		read_fds = master; // copy it
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}

		// run through the existing connections looking for data to read
		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) { // we got one!!
				if (i == consola_listener) {
					puts("Cambió el listener de consola\n");
					// handle new connections
					addrlen = sizeof remoteaddr;
					newfd = accept(consola_listener,
							(struct sockaddr *) &remoteaddr, &addrlen);

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
										get_in_addr(
												(struct sockaddr*) &remoteaddr),
										remoteIP, INET6_ADDRSTRLEN), newfd);

						if ((bytes_recibidos = recv(newfd, buf, 11, 0)) == -1) {
							perror("recv");
							exit(1);
						}

						printf("Se recibio: %s\nbytes_recibidos: %d.\n", buf,
								bytes_recibidos);

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
							(struct sockaddr *) &remoteaddr, &addrlen);

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
										get_in_addr(
												(struct sockaddr*) &remoteaddr),
										remoteIP, INET6_ADDRSTRLEN), newfd);

						printf("El fd es: %d", newfd);
						if ((bytes_recibidos = recv(newfd, buf, sizeof(buf), 0))
								== -1) {
							perror("recv");
							exit(1);
						}

						printf("Se recibio: %s\nbytes_recibidos: %d.\n", buf,
								bytes_recibidos);

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
					if (send(cpu_socket, buf, sizeof(buf), 0) == -1) {
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
