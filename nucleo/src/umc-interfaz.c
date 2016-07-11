#include "umc-interfaz.h"

int socket_umc;

int almacenamientoPosible(int canPaginas){
	char* paginas;
	char* cantPAginasSerializada = serializarInt(paginas, canPaginas);	//TODO: revisar. SerializarInt recibe un puntero a un entero

	sendMessage(socket_umc, HEADER_SOLICITAR_PAGINAS, sizeof(cantPAginasSerializada), cantPAginasSerializada);
	message* respuesta = receiveMessage(socket_umc);		//receiveMessage devuelve un message*

	if (respuesta->header == HEADER_PAGINAS_DISPONIBLES)return true;
	if (respuesta->header == HEADER_PAGINAS_NO_DISPONIBLES)return false;

	return false;
}


void notificarFinDePrograma(){
	sendMessage(socket_umc, HEADER_FIN_PROGRAMA, 0, "");
}


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
