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
#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <commons/config.h>
#include <commons/txt.h>
#include <commons/string.h>
#include "tlb.h"

	extern int cantidad_de_marcos;
	extern int marco_size;
	extern int marcos_x_proc;

	typedef struct tabla_de_frame_entry{
		int nroFrame;
		char* direccion_real;
		int referenciado; //valor de referencia para algoritmos de reemplazo Clock
		int ocupado;
	} tabla_de_frame_entry;

	typedef struct tabla_de_frames{
		t_list* entradas;
	} tabla_de_frames;

	typedef struct tabla_de_paginas_entry{
		int nroPagina;
		int presente;
		int modificado;
		int nroFrame;
	} tabla_de_paginas_entry;

	typedef struct tabla_de_paginas{
		t_list* entradas;
		int pid;
	} tabla_de_paginas;

	int initMemoriaPrincipal(t_config* config);

	char* initProgramaUMC(int pid, int cantPaginas);

	void escribirEnFrame(char* buffer, int offset, int tamanio, int frame);

	char* obtenerBytesDeMemoriaPrincipal(int frame, int offset, int tamanio);

	char* obtenerBytes(char* contenido, int offset, int tamanio);

	tabla_de_paginas* buscarPorPID(int pidABuscar);

	tabla_de_paginas_entry* buscarPorNroPaginaYPID(int nroPagina, int pid);

	void cargarPagina(int nroPagina, int pid, char* pagina);

#endif /* UMC_STRUCTS_H_ */
