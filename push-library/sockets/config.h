#ifndef NUCLEO_STRUCTS_H_
#define NUCLEO_STRUCTS_H_
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>

	//Obtiene archivo de configuracion
	//Si no se puede leer finaliza el proceso
	t_config* getConfig(char* fileName);

#endif
