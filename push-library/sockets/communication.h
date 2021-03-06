#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <parser/parser.h>
#include <commons/collections/list.h>

#include "pcb.h"
#include "serialization.h"
#include "error_codes.h"

	extern int32_t HEADER_SIZE ;
	extern int32_t RESPUESTA_SIZE;
	extern int32_t RESPUESTA_OK;
	extern int32_t RESPUESTA_FAIL;
	extern int32_t HEADER_INVALIDO;

	extern int32_t HEADER_HANDSHAKE;
	extern int32_t HEADER_INIT_PROGRAMA;
	extern int32_t HEADER_SOLICITAR_PAGINAS;
	extern int32_t HEADER_ALMACENAR_PAGINAS;
	extern int32_t HEADER_FIN_PROGRAMA;
	extern int32_t HEADER_RESULTADOS;
	extern int32_t HEADER_PAGINAS_DISPONIBLES;
	extern int32_t HEADER_PAGINAS_NO_DISPONIBLES;
	extern int32_t HEADER_ENVIAR_PCB;
	extern int32_t HEADER_NOTIFICAR_IO;
	extern int32_t HEADER_NOTIFICAR_FIN_QUANTUM;
	extern int32_t HEADER_NOTIFICAR_FIN_RAFAGA;
	extern int32_t HEADER_NOTIFICAR_WAIT;
	extern int32_t HEADER_NOTIFICAR_SIGNAL;
	extern int32_t HEADER_IMPRIMIR;
	extern int32_t HEADER_IMPRIMIR_TEXTO;
	extern int32_t HEADER_CAMBIO_PROCESO_ACTIVO;
	extern int32_t SIGUSR1;

	extern int32_t HEADER_WAIT_CONTINUAR;
	extern int32_t HEADER_WAIT_BLOQUEAR;
	extern int32_t HEADER_SETEAR_VARIABLE;
	extern int32_t HEADER_OBTENER_VARIABLE;

	extern int32_t HEADER_ENVIAR_QUANTUM;


	extern int32_t TIPO_CONSOLA;
	extern int32_t TIPO_NUCLEO;
	extern int32_t TIPO_UMC;
	extern int32_t TIPO_SWAP;
	extern int32_t TIPO_CPU;
	extern int32_t TIPO_SIZE;

	typedef struct message{
		int32_t header;
		int contenidoSize;
		char* contenido; //serializado
		int32_t codError; // if(-1) ok
	} message;


	void deleteMessage(message* message);
	message* receiveMessage(int socket);

	int32_t sendMessage(int socket, int header, int contenidoSize, char* contenidoSerializado);

	int32_t sendMessageInt(int socket, int header, int value);

	int32_t sendErrorMessage(int socket, int header, int errorCode);

	response* recibirResponse(int socket);

	//Creador de response con campo ok=1, el resto en 0
	response* createOKResponse();

	//Creador de response con campo ok=0, codError=codError, el resto en 0
	response* createFAILResponse(int codError);

	//Constructor de response con todos los campos customizables
	response* createResponse(int ok, int codError, int contenidoSize, char* contenido);

	void deleteResponse(response* response);

	int enviarOKSinContenido(int socket);

	int enviarOKConContenido(int socket, int contenidoSize, char* contenido);

	int enviarFAIL(int socket, int codError);

	void enviarResultado(response* response, int socket);

	char* serializarInt(char* posicionDeEscritura, int32_t* value);

//CPU a UMC
typedef struct cpu_to_umc {

	//Fijate si necesitas el PCB entero o si solo necesitas el processId.
	//PCB* pcb;
	//int processID;

	char* action;  //accion, puede ser define, set, get o remove
	t_puntero memoryAddr;
	t_nombre_variable value;
} cpu_to_umc;

//Inicializa una variable y debe devolver la posicion de memoria.
t_puntero cpu_to_umc_define(t_valor_variable var);

//Obtiene el valor de una variable a partir de la posicion de memoria.
t_valor_variable cpu_to_umc_get(t_puntero var);

//Guarda una variable en memoryAddr con el valor var.
void cpu_to_umc_set(t_puntero memoryAddr, t_valor_variable var);

//Elimina una posicion de memoria
void cpu_to_umc_remove(t_puntero memoryAddr);




//NUCLEO a CPU
typedef struct nucleo_to_cpu {

	PCB* pcb;
	t_list list; //array de char* de las instrucciones que hay que ejecutar.
} nucleo_to_cpu;


#endif
