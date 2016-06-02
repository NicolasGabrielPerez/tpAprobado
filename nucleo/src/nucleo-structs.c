#include "nucleo-structs.h"

int quantum;
int quantum_sleep ;
char** io_ids;
char** io_sleep_times;

void initNucleo(t_config* config){
    quantum = config_get_int_value(config, "QUANTUM");
    quantum_sleep = config_get_int_value(config, "QUANTUM_SLEEP");
	io_ids = config_get_array_value(config, "IO_ID");
	io_sleep_times = config_get_array_value(config, "IO_SLEEP");
    //char** semaforos_ids = config_get_array_value(config, "SEM_ID");
	//char** semaforos_init_values = config_get_array_value(config, "SEM_INIT");
	//char** shared_values = config_get_array_value(config, "SHARED_VARS");
}
