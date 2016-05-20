#include "estructuras.h"

Pagina* crear_pagina(char* contenido, int32_t nroPagina, int32_t pid){
	Pagina* pagina = malloc(sizeof(Pagina));
	pagina->contenido = contenido;
	pagina->nroPagina = nroPagina;
	pagina->pid = pid;

	return pagina;
}
