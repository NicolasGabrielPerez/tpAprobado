#include "nucleo-structs.h"
#include "consola-interfaz.h"
#include "cpu-interfaz.h"
#include "umc-interfaz.h"
#include "planificador.h"
#include "testing-serialization.h"
#include <pthread.h>
#include <commons/log.h>

pthread_attr_t nucleo_attr;
t_log* nucleo_logger;

int main(void) {

	//test_planification();
	//test_serialization();

	///*
	t_config* config = getConfig("nucleo.config");

	initNucleo(config); //lee valores de quantum y io
	initCPUListener(config);
	initConsolaListener(config);
	conectarConUMC(config); //conecta con UMC y hace handshake

	log_create("nucleo_log.txt", "Núcleo", true, LOG_LEVEL_TRACE);

	pthread_attr_init(&nucleo_attr);
	pthread_attr_setdetachstate(&nucleo_attr, PTHREAD_CREATE_DETACHED);

	pthread_t cpuCommunicationThread;
	pthread_create(&cpuCommunicationThread, &nucleo_attr, &cpu_comunication_program, NULL);

	pthread_t consoleCommunicationThread;
	pthread_create(&consoleCommunicationThread, &nucleo_attr, &console_comunication_program, NULL);

	pthread_t fileObserverThread;
	pthread_create(&fileObserverThread, &nucleo_attr, &hiloDeLectura, config);

	pthread_t plannificationThread;		//Hilo de planificación
	pthread_create(&plannificationThread, &nucleo_attr, &plannificationProgram, config);
//*/

	return EXIT_SUCCESS;
}
