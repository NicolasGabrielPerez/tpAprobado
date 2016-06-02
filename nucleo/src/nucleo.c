#include "nucleo-structs.h"
#include "consola-interfaz.h"
#include "cpu-interfaz.h"
#include "umc-interfaz.h"

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
	t_config* config = getConfig("nucleo.config");

	initNucleo(config); //lee valores de quantum y io
	initCPUListener(config);
	initConsolaListener(config);
	conectarConUMC(config); //conecta con UMC y hace handshake

	int bytes_recibidos;
	int newfd;        // newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;

	char buf[50];    // buffer for client data
	int nbytes;

	int yes=1;        // for setsockopt() SO_REUSEADDR, below

	for(;;) {
		manejarConsolas();
		//manejarCpus();
	}

	return 0;
}
