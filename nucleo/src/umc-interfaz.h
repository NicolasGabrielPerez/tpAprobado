#ifndef UMC_INTERFAZ_H_
#define UMC_INTERFAZ_H_
#include "nucleo-structs.h"
#include <sockets/communication.h>

extern int socket_umc;

void swichRecivirPorHEADER();

void conectarConUMC(t_config* config);

void notificarFinDePrograma();

int almacenamientoPosible(int canPaginas);

//TODO: Pedír páginas de memoria para programa y contestar
#endif
