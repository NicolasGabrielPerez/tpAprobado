#ifndef UMC_STRUCTS_H_
#define UMC_STRUCTS_H_

typedef struct{
	int nroFrame;
	char* direccion_real;
	int pid;
	int referenciado; //valor de referencia para algoritmos de reemplazo Clock
	int ocupado;
} umc_frame;

typedef struct{
	int nroPagina;
	int presente;
	int modificado;
	int nroFrame;
} tabla_de_paginas_entry;

typedef struct{
	tabla_de_paginas_entry* entradas;
} tabla_de_paginas;

#endif /* UMC_STRUCTS_H_ */
