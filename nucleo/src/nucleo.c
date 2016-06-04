#include "nucleo-structs.h"
#include "consola-interfaz.h"
#include "cpu-interfaz.h"
#include "umc-interfaz.h"

int main(void) {
	t_config* config = getConfig("nucleo.config");

	initNucleo(config); //lee valores de quantum y io
	initCPUListener(config);
	initConsolaListener(config);
	conectarConUMC(config); //conecta con UMC y hace handshake

	while(true) {
		manejarConexionesConsolas();
		manejarConexionesCPUs();
	}

	return EXIT_SUCCESS;
}
