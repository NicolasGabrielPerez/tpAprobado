#include "configInit.h"


char* PATH_CONFIG = "/home/utnso/Dropbox/Facultad/UTN/Sistemas Operativos/compartido VM/workspace/TP/CPU/src/config.txt";

void initConfig() {

	t_config* config = config_create(PATH_CONFIG);

	char* property = "property";
	if(config_has_property(config, property)) {
		printf("Si tiene");
	} else {
		printf("No tiene");
	}
}
