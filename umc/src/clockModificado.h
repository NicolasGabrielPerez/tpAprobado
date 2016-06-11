#ifndef CLOCK_MODIFICADO_H_
#define CLOCK_MODIFICADO_H_

#include "umc-structs.h"

	extern int agujaClockModificado; //nroFrame apuntado actualmente

	void initClockModificado();

	response* clockModificadoGetFrame(int nroPagina, int pidActivo);

#endif
