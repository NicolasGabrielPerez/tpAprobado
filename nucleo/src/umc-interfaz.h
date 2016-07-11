#ifndef UMC_INTERFAZ_H_
#define UMC_INTERFAZ_H_
#include "nucleo-structs.h"

	extern int socket_umc;

	void conectarConUMC(t_config* config);

	void notificarFinDePrograma();

	void almacenamientoPosible(int canPaginas);



	//TODO: Pedír páginas de memoria para programa y contestar
#endif
