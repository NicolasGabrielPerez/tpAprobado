#ifndef CLOCK_H_
#define CLOCK_H_

#include "umc-structs.h"

	extern int agujaClock; //nroFrame apuntado actualmente

	void initClock();

	//Objetivo de esta funcion:
	// Obtener nroFrame de pagina a usar (escribir/leer)
	// Internamente interactua son Swap
	response* clockGetFrame(int nroPagina, int pidActivo);

#endif
