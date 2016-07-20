#ifndef UMC_STRUCTS_H_
#define UMC_STRUCTS_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sockets/sockets.h>
#include <sockets/communication.h>
#include <sockets/serialization.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <commons/config.h>
#include <commons/txt.h>
#include <commons/string.h>
#include "tlb.h"
#include <commons/log.h>

	extern int cantidad_de_marcos;
	extern int marco_size;
	extern int marcos_x_proc;

	extern char* memoria_bloque;

	extern int retardo;

	extern t_log* logger;

	enum AlgoritmoReemplazo{
		clockComun,
		clockModificado
	};

	extern enum AlgoritmoReemplazo algoritmoActivo;

	typedef struct tabla_de_frame_entry{
		int nroFrame;
		char* direccion_real;
		int ocupado;
	} tabla_de_frame_entry;

	typedef struct tabla_de_frames{
		t_list* entradas;
	} tabla_de_frames;

	typedef struct tabla_de_paginas_entry{
		int nroPagina;
		int presente;
		int modificado;
		int uso;
		int nroFrame;
		int pid;
	} tabla_de_paginas_entry;

	typedef struct presente{
		int nroPagina;
		int nroFrame;
		int uso;
		int modificado;
	} presente;

	typedef struct tabla_de_paginas{
		t_list* entradas;
		int pid;
		int aguja;
		t_list* presentes;
	} tabla_de_paginas;

	//Estrucutra usada para funciones internas de UMC (pasaje entre cpu-interfaz y clock, etc)
	typedef struct umcResult{
		int32_t ok;
		int32_t codError;
		tabla_de_paginas_entry* paginaEntry;
		tabla_de_frame_entry* frameEntry;
	} umcResult;

	t_list* tablasDePaginas;

	pthread_attr_t nucleo_attr;

	umcResult createUmcResult(int ok, int codError, tabla_de_paginas_entry* paginaEntry, tabla_de_frame_entry* frameEntry);

	umcResult createFAILResult(int codError);

	umcResult createOkPageResult(tabla_de_paginas_entry* paginaEntry);

	umcResult createOkFrameResult(tabla_de_frame_entry* frameEntry);

	int initMemoriaPrincipal(t_config* config);

	response* initProgramaUMC(int pid, int cantPaginas);

	void escribirEnFrame(char* buffer, int offset, int tamanio, int frame);

	char* obtenerBytesDeMemoriaPrincipal(int frame, int offset, int tamanio);

	char* obtenerBytes(char* contenido, int offset, int tamanio);

	tabla_de_paginas* buscarPorPID(int pidABuscar);

	tabla_de_paginas_entry* buscarPorNroPaginaYPID(int nroPagina, int pid);

	void cargarPagina(int nroPagina, int pid, char* pagina);

	char* leerFrame(int nroFrame);

	int buscarPIDActualDeFrame(int nroFrame);

	int buscarNroPagActualDeFrame(int nroFrame);

	int obtenerFrameDisponible();

	void cargarEnMemoriaPrincipal(char* pagina, int nroFrame);

	tabla_de_frame_entry* obtenerEntradaDeFrame(int nroFrame);

	void initLogger();

	response* finalizarPidDeUMC(int pid);

	#endif /* UMC_STRUCTS_H_ */
