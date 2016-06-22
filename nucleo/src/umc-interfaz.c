#include "umc-interfaz.h"
#include <sockets/communication.h>

int socket_umc ;

void conectarConUMC(t_config* config){
	 char* puerto_umc = config_get_string_value(config, "PUERTO_UMC"); //puerto de UMC
	 socket_umc = crear_socket_cliente("utnso40", puerto_umc);

	 //TODO: crear función para enviar header
	 //Handshake con UMC
	 char* content = string_itoa(HEADER_HANDSHAKE);
	 int32_t size = sizeof(int32_t);
	 enviarOKConContenido(socket_umc, size, content);

	 //Tipo de handshake
	 content = string_itoa(TIPO_NUCLEO);
	 size = sizeof(int32_t);
	 enviarOKConContenido(socket_umc, size, content);

	 //Recibir tamaño de página
	 response* UmcResponse = recibirResponse(socket_umc);
	 if(UmcResponse->ok){
		 memoryPageSize = convertToInt32(UmcResponse->contenido);
	 }
	 else{
		 puts("Tamaño de página no recibido");
	 }
}
