#include "config.h"

t_config* getConfig(char* fileName){
	t_config* config = config_create(fileName);
	if(config==NULL){
		printf("No se pudo leer la configuración");
		exit(1);
	}
	return config;
}
