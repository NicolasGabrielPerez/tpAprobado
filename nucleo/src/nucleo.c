#include "nucleo-structs.h"
#include "consola-interfaz.h"
#include "cpu-interfaz.h"
#include "umc-interfaz.h"
#include "planificador.h"
#include "testing-serialization.h"
#include <pthread.h>
#include <commons/log.h>
#include <sys/types.h>
#include <sys/inotify.h>

// El tamaño de un evento es igual al tamaño de la estructura de inotify
// mas el tamaño maximo de nombre de archivo que nosotros soportemos
// en este caso el tamaño de nombre maximo que vamos a manejar es de 24
// caracteres. Esto es porque la estructura inotify_event tiene un array
// sin dimension ( Ver C-Talks I - ANSI C ).
#define EVENT_SIZE  ( sizeof (struct inotify_event) + 24 )

// El tamaño del buffer es igual a la cantidad maxima de eventos simultaneos
// que quiero manejar por el tamaño de cada uno de los eventos. En este caso
// Puedo manejar hasta 1024 eventos simultaneos.
#define BUF_LEN     ( 1024 * EVENT_SIZE )

pthread_attr_t nucleo_attr;
t_log* nucleo_logger;

int main(void) {
	t_config* config = getConfig("nucleo.config");
	initNucleo(config); //lee valores de quantum y io

	if(!test_mode){
		nucleo_logger = log_create("nucleo_log.txt", "Núcleo", true, LOG_LEVEL_TRACE);

		com_initCPUListener(config);
		com_initConsolaListener(config);
		conectarConUMC(config); 			//conecta con UMC y hace handshake

		init_threads_config(nucleo_attr);
		init_cpu_communication_thread(nucleo_attr);
		init_console_communication_thread(nucleo_attr);
		init_planification_thread(nucleo_attr);

		//test_cpu_communication();


		char buffer[BUF_LEN];

		// Al inicializar inotify este nos devuelve un descriptor de archivo
		int file_descriptor = inotify_init();
		if (file_descriptor < 0) {
			perror("inotify_init");
		}

		// Creamos un monitor sobre un path indicando que eventos queremos escuchar
		int watch_descriptor = inotify_add_watch(file_descriptor, "nucleo.config", IN_CLOSE_WRITE | IN_MODIFY );

		while(1){
			// El file descriptor creado por inotify, es el que recibe la información sobre los eventos ocurridos
			// para leer esta información el descriptor se lee como si fuera un archivo comun y corriente pero
			// la diferencia esta en que lo que leemos no es el contenido de un archivo sino la información
			// referente a los eventos ocurridos
			int length = read(file_descriptor, buffer, BUF_LEN);
			if (length < 0) {
				perror("read");
			}

			int offset = 0;

			// Luego del read buffer es un array de n posiciones donde cada posición contiene
			// un eventos ( inotify_event ) junto con el nombre de este.
			while (offset < length) {

				// El buffer es de tipo array de char, o array de bytes. Esto es porque como los
				// nombres pueden tener nombres mas cortos que 24 caracteres el tamaño va a ser menor
				// a sizeof( struct inotify_event ) + 24.
				struct inotify_event *event = (struct inotify_event *) &buffer[offset];

				// El campo "len" nos indica la longitud del tamaño del nombre
				if (event->mask & IN_CLOSE_WRITE) {

					//create config otra vez
					//chequear que el campo haya cambiado (quantum o lo que sea)
					t_config* config = getConfig("nucleo.config");
					quantum = config_get_int_value(config, "QUANTUM");
					quantum_sleep = config_get_int_value(config, "QUANTUM_SLEEP");
				}
				if (event->mask & IN_MODIFY) {

					//create config otra vez
					//chequear que el campo haya cambiado (quantum o lo que sea)
					t_config* config = getConfig("nucleo.config");
					quantum = config_get_int_value(config, "QUANTUM");
					quantum_sleep = config_get_int_value(config, "QUANTUM_SLEEP");
				}
				offset += sizeof (struct inotify_event) + event->len;
			}
		}
	}
	else{
		//test_PCB_serialization();
		//test_planification();
		//test_serialization();
		test_list_and_queues();
	}
	return EXIT_SUCCESS;
}
