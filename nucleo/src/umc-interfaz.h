#ifndef UMC_INTERFAZ_H_
#define UMC_INTERFAZ_H_
#include "nucleo-structs.h"
#include <sockets/communication.h>

extern int socket_umc;

void conectarConUMC(t_config* config);

void notificarFinDePrograma();

int almacenamientoPosible(int canPaginas);

void umc_process_active(int processId); //Settea el proceso activo.
void umc_set(t_puntero page, t_puntero offset, u_int32_t size, char* buffer); 	//Setea el valor a una variable.
t_valor_variable umc_get(t_puntero page, t_puntero offset, u_int32_t size); 	//Obtiene el valor de una variable.

//TODO: Pedír páginas de memoria para programa y contestar
#endif
