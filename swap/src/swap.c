#include "swap-structs.h"
#include "interfazUmc.h"
#include "pages_admin.h"

void operarSegunHeader(int32_t header){

	if(header == HEADER_INIT_PROGRAMA){
		log_info(logger, "Recibido Header Init Programa");
		recibirInitPrograma();
		return;
	}
	if(header == HEADER_SOLICITAR_PAGINAS){
		log_info(logger, "Recibido Header Solicitar Paginas");
		recibirPedidoPagina();
		return;
	}
	if(header == HEADER_ALMACENAR_PAGINAS){
		log_info(logger, "Recibido Header Almacenar Paginas");
		recibirEscrituraPagina();
		return;
	}
	if(header == HEADER_FIN_PROGRAMA){
		log_info(logger, "Recibido Header Fin Programa");
		recibirFinPrograma(); //TODO revisar
		return;
	}
}

int main(void) {
	t_config* config = config_create("swap.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}

	initLogger();

	initSwap(config);

	initUmc(config);

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
		   printf("UMC hung up\n");
		   break;
	   }

	   memcpy(&headerInt, header, sizeof(int32_t));
	   operarSegunHeader(headerInt);
	}

	close(umc_socket); // bye!

	printf("Swap finalizada.\n");
	return EXIT_SUCCESS;
}
