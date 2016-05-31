#ifndef CPU_INTERFAZ_H_
#define CPU_INTERFAZ_H_
#include "umc-structs.h"
#include <sockets/communication.h>

	void recibirAlmacenarPaginas(int cpu_socket, int pidActivo);

	void recibirSolicitarPaginas(int cpu_socket, int pidActivo);

	void recibirCambioDeProcesoActivo(int cpu_socket, int* pidActivo);

#endif /* CPU_INTERFAZ_H_ */
