#ifndef PAGES_REPLACEMENT_H_
#define PAGES_REPLACEMENT_H_

#include "umc-structs.h"

	enum AlgoritmoReemplazo{
		clockComun,
		clockModificado
	};

	extern enum AlgoritmoReemplazo algoritmoActivo;

	umcResult getPageEntry(tabla_de_paginas* tablaDePaginas, int nroPagina);

#endif
