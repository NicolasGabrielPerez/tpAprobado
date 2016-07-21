/*
 * Copyright (C) 2012 Sistemas Operativos - UTN FRBA. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SWAP_STRUCTS_H_
#define SWAP_STRUCTS_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sockets/sockets.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <commons/config.h>
#include <commons/txt.h>
#include <commons/string.h>
#include <commons/log.h>

	extern t_log* logger;
	extern int retardo;

	typedef struct frame_entry{
		int nroFrame;
		int pid;
		int nroPagina;
	} frame_entry;

	typedef struct swap_admin{
		t_list* framesEntries;
		t_bitarray* bitMap;
		FILE* particion;
	} swap_admin;

	extern int paginaSize;
	extern int cantPaginasSwap;
	extern swap_admin* swapAdmin;

	/**
	* @NAME: initSwap
	* Lee configuracion
	* Crea la estructura swap_admin
	*/
	int initSwap(t_config* config);

	frame_entry* buscarFrameEntry(int nroPagina, int pid);

	frame_entry* getFrameEntryPorNroFrame(int nroFrame);

	t_list* buscarEntries(int pid);

	void demorarSolicitud();

	void demorarCompactacion();

#endif /* SWAP_STRUCTS_H_ */
