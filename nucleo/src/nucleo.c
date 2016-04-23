#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#define PUERTO "7001"

#define PACKAGESIZE 1024	// Define cual va a ser el size maximo del paquete a enviar
#define BACKLOG 5			// Define cuantas conexiones vamos a mantener pendientes al mismo tiempo

t_dictionary* create_dictionary_from_strings(char** keys, char** values){
	t_dictionary* dictionary = dictionary_create();
	int i;
	for(i=0 ; keys[i] != NULL ; i++){
		dictionary_put(dictionary, keys[i], values[i]);
	}
	return dictionary;
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
    t_dictionary* semaforo_dictionary = create_dictionary_from_strings(semaforos_ids, semaforos_init_values); //lo creo por las dudas...

    char** shared_values = config_get_array_value(config, "SHARED_VARS");



    struct addrinfo hints;
    struct addrinfo *serverInfo;

    	memset(&hints, 0, sizeof(hints));
    	hints.ai_family = AF_INET;		//Delego en la máquina la elección entre IPv4 o IPv6
    	hints.ai_flags = AI_PASSIVE;		//Asigna el address del localhost: 127.0.0.1
    	hints.ai_socktype = SOCK_STREAM;	//Usamos protocolo TCP

    	getaddrinfo(NULL, puerto_cpu, &hints, &serverInfo);		//Paso NULL en el parámetro de IP porque antes indiqué que use el localhost en AI_PASSIVE

    	//Necesito conocer cuáles serán las conexiones que quieren establecerse con el server
    	int listeningSocket;
    	listeningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

    	//Asociar el socket de escucha a un puerto
    	bind(listeningSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
    	freeaddrinfo(serverInfo);

    	//Escuchar
    	printf("Proceso bloqueado a la espera de un cliente\n");
    	listen(listeningSocket, backlog);		//listen() es una llamada al sistema bloqueante

    	//El sistema esperará hasta que reciba una conexión entrante
    	//Aceptar conexión entrante y crear un nuevo socket
    	//Nota: Para que el listenningSocket vuelva a esperar conexiones, necesitariamos volver a decirle que escuche, con listen();
    	 //*				En este ejemplo nos dedicamos unicamente a trabajar con el cliente y no escuchamos mas conexiones.

    	struct sockaddr_in addr;			//Estructura que contendrá los datos de la conexión del cliente
    	socklen_t addrlen = sizeof(addr);

    	int socketCliente = accept(listeningSocket, (struct sockaddr *) &addr, &addrlen);


    	//Espero bloqueado los mensajes del cliente
    	//Cuando el cliente cierra la conexion, recv() devolvera 0.
    	char package[package_size];
    	int status = 1;						//Maneja el estado de los recieve

    	printf("Cliente conectado. Esperando mensajes:\n");

    	while(status != 0){
    		status = recv(socketCliente, (void*) package, package_size, 0);
    		if(status != 0)
    			printf("%s", package);
    	}

    	//Terminado el intercambio de paquetes, cerramos todas las conexiones
    	close(socketCliente);
    	close(listeningSocket);


	return EXIT_SUCCESS;
}
