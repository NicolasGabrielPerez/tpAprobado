#ifndef UMC_STRUCTS_H_
#define UMC_STRUCTS_H_

typedef struct{
	int nroFrame;
	char* direccion_real;
	int pid;
	int referenciado; //valor de referencia para algoritmos de reemplazo Clock
	int ocupado;
} tabla_de_frame_entry;

typedef struct{
	tabla_de_frame_entry* entradas;
} tabla_de_frames;

typedef struct{
	int nroPagina;
	int presente;
	int modificado;
	int nroFrame;
} tabla_de_paginas_entry;

typedef struct{
	tabla_de_paginas_entry* entradas;
} tabla_de_paginas;

typedef struct{
	int pid;
	int cantPaginas;
	char* codigoFuente;
} pedido_init;

#endif /* UMC_STRUCTS_H_ */
