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

	test_PCB_serialization();
	//test_planification();
	//test_serialization();

	/*
	t_config* config = getConfig("nucleo.config");

	//*
	nucleo_logger = log_create("nucleo_log.txt", "Núcleo", true, LOG_LEVEL_TRACE);

	initNucleo(config); //lee valores de quantum y io
	com_initCPUListener(config);
	com_initConsolaListener(config);
	conectarConUMC(config); //conecta con UMC y hace handshake


	init_threads_config(nucleo_attr);
	init_cpu_communication_thread(nucleo_attr);
	init_console_communication_thread(nucleo_attr);
	init_planification_thread(nucleo_attr);

	//test_cpu_communication();

	while(1){
		usleep(500000);
		//PLanificación

		/*usleep(500000);
		t_CPU* freeCPU = get_next_free_CPU();
		if(freeCPU != NULL){
			if(!queue_is_empty(READY_Process_Queue)){
				set_next_pcb_RUNNING(freeCPU->cpuSocket);
			}
		}
	}
	//*/
	return EXIT_SUCCESS;
}
