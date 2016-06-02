#include "consola-interfaz.h"

int consola_listener;
fd_set consola_sockets_set;
int fd_consola_max;

void initConsolaListener(t_config* config){
	char* puerto_prog = config_get_string_value(config, "PUERTO_PROG");
	consola_listener = crear_puerto_escucha(puerto_prog);
	FD_ZERO(&consola_sockets_set);    // clear the master and temp sets
	FD_SET(consola_listener, &consola_sockets_set);
}

void manejarConexionesConsolas(){
	fd_set read_fds; //set auxiliar
	read_fds = consola_sockets_set; // copy it
	if (select(fd_consola_max+1, &read_fds, NULL, NULL, NULL) == -1) {
	   perror("select");
	   exit(4);
	}

	// run through the existing connections looking for data to read
	int i;
	for(i = 0; i <= fd_consola_max; i++) {
	   if (FD_ISSET(i, &read_fds)) {
		   if (i == consola_listener) { //nueva consola
				aceptarNuevaConexion(consola_listener);
				//make handshake
				continue;
		   } else{
			   handleData(i);
			   continue;
		   }
	   };
	} //ends FOR
}

void handleData(int consola_socket){

}
