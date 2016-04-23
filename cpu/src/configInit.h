#ifndef CONFIGINIT_H_
#define CONFIGINIT_H_

	#include <stdio.h>
	#include <commons/config.h>


	#define NUCLEO_IP "127.0.0.1"
	#define NUCLEO_PORT "8000"
	#define UMC_IP "127.0.0.1"
	#define UMC_PORT "8009"

	#define PACKAGESIZE 1024 //Esto es variable segun la cantidad de datos.

	void initConfig();

#endif /* CONFIGINIT_H_ */
