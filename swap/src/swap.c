#include "swap-structs.h"
#include "interfazUmc.h"
#include "pages_admin.h"

void operarSegunHeader(int32_t header){
	if(header == HEADER_HANDSHAKE){
		makeHandshake();
		return;
	}
	if(header == HEADER_INIT_PROGRAMA){
		recibirInitPrograma();
		return;
	}
	if(header == HEADER_SOLICITAR_PAGINAS){
		recibirPedidoPagina();
		return;
	}
	if(header == HEADER_ALMACENAR_PAGINAS){
		recibirEscrituraPagina();
		return;
	}
	if(header == HEADER_FIN_PROGRAMA){
		recibirFinPrograma();
		return;
	}
}

int main(void) {
	t_config* config = config_create("swap.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}

	int init_swap = initSwap(config);
	if(init_swap==-1) return EXIT_FAILURE;

	int init_umc = initUmc(config);
	if(init_umc==-1) return EXIT_FAILURE;

	int bytes_recibidos = 1;
	char* header = malloc(HEADER_SIZE);
	int32_t headerInt;
	while(bytes_recibidos){
		printf("Esperando conexiones...\n");

		bytes_recibidos = recv(umc_socket, header, HEADER_SIZE, 0);

		if(bytes_recibidos == -1) {
		   perror("recv");
		   exit(1);
		}

		 if (bytes_recibidos == 0) {
		   // connection closed
		   printf("umc hung up\n");
	   }

	   memcpy(&headerInt, header, sizeof(int32_t));
	   operarSegunHeader(headerInt);
	}

	close(umc_socket); // bye!

	puts("Terminé felizmente\n");
	return EXIT_SUCCESS;
}
