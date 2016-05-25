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
#include <stdio.h>
#include <stdlib.h>

#include "umcFunctions.h"

int socket_umc;

void umc_init(t_config* config){

	char* puerto_umc = config_get_string_value(config, "PUERTO_UMC");
	printf("Config: PUERTO_UMC=%s\n", puerto_umc);

	socket_umc = crear_socket_cliente("utnso40", puerto_umc); //socket usado para conectarse a la umc
	printf("UMC FD: %d\n", socket_umc);

	//Hago handshake con umc
	if(handshake(socket_umc, "PRUEBA") != 0){
		puts("Error en handshake con la umc");
	}
}

void umc_delete() {

}

void umc_set(t_puntero page, t_puntero offset, u_int32_t size) {


}

char* umc_get(t_puntero page, t_puntero offset, u_int32_t size, char* buffer) {

	char* result = "resultado";
	return result;
}
