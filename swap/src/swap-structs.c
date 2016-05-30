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
#include "swap-structs.h"

#include <stdio.h>
#include <stdlib.h>

int paginaSize;
int cantPaginas;
int swapSize; //en bytes
int retardo_fragmentacion;
char* particionFileName;
FILE* particion;

swap_admin* swapAdmin;

void setConfig(t_config* config){
	paginaSize = config_get_int_value(config, "TAMANIO_PAGINA");
	cantPaginas = config_get_int_value(config, "CANTIDAD_PAGINAS");
	retardo_fragmentacion = config_get_int_value(config, "RETARDO_COMPACTACION");
	particionFileName = config_get_string_value(config, "NOMBRE_PARTICION");
	swapSize = cantPaginas * paginaSize;
}

char* generarComandoDD(){
	char* command = "dd if=/dev/zero of=";
	string_append(&command, particionFileName);
	string_append(&command, " bs=");
	string_append(&command, string_itoa(paginaSize));
	string_append(&command, " count=");
	string_append(&command, string_itoa(cantPaginas));
	return command;
}

void crearPartcion(){
	system(generarComandoDD());
}

int crearParticion(){
	crearPartcion();
	particion = txt_open_for_append(particionFileName);
	txt_write_in_file(particion, '\0');

	return EXIT_SUCCESS;
}

void crearBitMap(){
	char* bitMap = malloc(swapSize);
	swapAdmin->bitMap = bitarray_create(bitMap, swapSize);
}

void crearFrames(){
	t_list* framesEntries = list_create();
	int i;
	for(i=0; i<cantPaginas; i++){
		frame_entry* frameEntry = malloc(sizeof(frame_entry));
		frameEntry->nroFrame = i;
		list_add(framesEntries, frameEntry);
	}
	swapAdmin->framesEntries = framesEntries;
}

int crearAdminStructs(){
	swapAdmin = malloc(sizeof(swap_admin));
	crearBitMap(swapAdmin);
	crearFrames(swapAdmin);

	return EXIT_SUCCESS;
}

int initSwap(t_config* config){

	setConfig(config);

	if(crearParticion() == EXIT_FAILURE) return EXIT_FAILURE;

	if(crearAdminStructs() == EXIT_FAILURE) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
