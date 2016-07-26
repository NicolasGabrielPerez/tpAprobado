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
int cantPaginasSwap;
int swapSize; //en bytes
int retardo_fragmentacion;
char* particionFileName;
int retardo;
swap_admin* swapAdmin;

void demorarCompactacion()
{
   struct timespec req, rem;

   if(retardo_fragmentacion > 999)
   {
        req.tv_sec = (int)(retardo_fragmentacion / 1000);                            /* Must be Non-Negative */
        req.tv_nsec = (retardo_fragmentacion - ((long)req.tv_sec * 1000)) * 1000000; /* Must be in range of 0 to 999999999 */
   }
   else
   {
        req.tv_sec = 0;                         /* Must be Non-Negative */
        req.tv_nsec = retardo_fragmentacion * 1000000;    /* Must be in range of 0 to 999999999 */
   }

   nanosleep(&req , &rem);
}

void demorarSolicitud()
{
   struct timespec req, rem;

   if(retardo > 999)
   {
        req.tv_sec = (int)(retardo / 1000);                            /* Must be Non-Negative */
        req.tv_nsec = (retardo - ((long)req.tv_sec * 1000)) * 1000000; /* Must be in range of 0 to 999999999 */
   }
   else
   {
        req.tv_sec = 0;                         /* Must be Non-Negative */
        req.tv_nsec = retardo * 1000000;    /* Must be in range of 0 to 999999999 */
   }

   nanosleep(&req , &rem);
}

void setConfig(t_config* config){
	paginaSize = config_get_int_value(config, "TAMANIO_PAGINA");
	cantPaginasSwap = config_get_int_value(config, "CANTIDAD_PAGINAS");
	retardo_fragmentacion = config_get_int_value(config, "RETARDO_COMPACTACION");
	particionFileName = config_get_string_value(config, "NOMBRE_PARTICION");
	swapSize = cantPaginasSwap * paginaSize;
	retardo = config_get_int_value(config, "RETARDO_ACCESO");
}

char* generarComandoDDSwap(){
	char* command = string_new();
	string_append(&command, "dd if=/dev/zero of=");
	string_append(&command, particionFileName);
	string_append(&command, " bs=");
	string_append(&command, string_itoa(paginaSize));
	string_append(&command, " count=");
	string_append(&command, string_itoa(cantPaginasSwap));
	return command;
}

char* generarComandoDD(char* particionFileName, int paginaSize, int cantPaginas){
	char* command = string_new();
	string_append(&command, "dd if=/dev/zero of=");
	string_append(&command, particionFileName);
	string_append(&command, " bs=");

	char* pageSize = string_itoa(paginaSize);
	string_append(&command, pageSize);

	string_append(&command, " count=");

	char* pageString = string_itoa(cantPaginas);
	string_append(&command, pageString);

	free(pageSize);
	free(pageString);
	return command;
}

int pruebaCrearArchivo(){
	//Set de datos
	char* nombreDeArchivo = "archivo";
	int cantPaginas = 2;
	int pageSize = 4;
	char* writeData = malloc(512);
	char* data = "ABCDED";
	memcpy(writeData, data, 5);
	char* readData = malloc(5);
	int size = strlen(writeData);

	//Crear archivo
	int result = system(generarComandoDD(nombreDeArchivo, pageSize, cantPaginas)); //2 paginas de 4 bytes cada una
	if(result == -1){
		printf("Fallo al crear particion\n");
		return EXIT_FAILURE;
	}

	//Abrir archivo
	FILE* file = fopen(nombreDeArchivo, "r+");
	if(file == NULL) return EXIT_FAILURE;
	int seekResult = fseek(file, 0, SEEK_SET);

	//Escribir caracteres
	int writeResult;
	writeResult = fwrite(writeData, sizeof(char), 512, file);
	fflush(file);
	printf("Write result: %d\n", writeResult);

	//Mostrar posicion actual
	int currentPos = ftell(file);
	printf("Current file position: %d\n", currentPos);

	//Leer caracteres
	printf("Reading...\n");
	currentPos = fseek(file, 0, SEEK_SET);
	printf("Current file position: %d\n", currentPos);
	int readResult = fread(readData, sizeof(char), 5, file);
	printf("Read result: %d\n", readResult);
	printf("Read data: %s\n", readData);

	fclose(size);
	return EXIT_SUCCESS;
}

int crearParticion(){
	//Crear archivo
	char* command = generarComandoDD(particionFileName, paginaSize, cantPaginasSwap);
	int result = system(command);
	if(result == -1){
		printf("Fallo al crear particion\n");
		return EXIT_FAILURE;
	}
	log_trace(logger, "Particion <%s> creada! Marcos disponibles:%d, tamanio de marco: %d. Total de bytes:%d", particionFileName, cantPaginasSwap, paginaSize, cantPaginasSwap*paginaSize);

	//Abrir archivo
	FILE* file = fopen(particionFileName, "r+");
	if(file == NULL){
		log_error(logger, "Error al intentar abrir archivo de particion");
		exit(1);
		return EXIT_FAILURE;
	}
	fseek(file, 0, SEEK_SET);
	swapAdmin->particion = file;

	free(particionFileName);
	free(command);

	return EXIT_SUCCESS;
}

void crearBitMap(){
	char* bitMap = malloc(cantPaginasSwap);
	swapAdmin->bitMap = bitarray_create(bitMap, cantPaginasSwap);
	int i;
	for(i=0; i<cantPaginasSwap; i++){
		bitarray_clean_bit(swapAdmin->bitMap, i);
	}

	log_trace(logger, "Cread bitmap. Cantiad de bits: %d", bitarray_get_max_bit(swapAdmin->bitMap));
}

void crearFramesTable(){
	t_list* framesEntries = list_create();
	int i;
	for(i=0; i<cantPaginasSwap; i++){
		frame_entry* frameEntry = malloc(sizeof(frame_entry));
		frameEntry->nroFrame = i;
		frameEntry->pid = 0;
		frameEntry->nroPagina = 0;
		list_add(framesEntries, frameEntry);
	}
	swapAdmin->framesEntries = framesEntries;
	log_trace(logger, "Creada table de frames. Cantidad de entradas: %d", list_size(swapAdmin->framesEntries));
}

int crearAdminStructs(){
	swapAdmin = malloc(sizeof(swap_admin));
	crearBitMap();
	crearFramesTable();

	return EXIT_SUCCESS;
}

int initSwap(t_config* config){
	setConfig(config);

	if(crearAdminStructs() == EXIT_FAILURE) return EXIT_FAILURE;
	if(crearParticion() == EXIT_FAILURE) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

frame_entry* getFrameEntryPorNroFrame(int nroFrame){
	int i;
	for(i=0;i<list_size(swapAdmin->framesEntries);i++){
		frame_entry* actual = list_get(swapAdmin->framesEntries, i);
		if(actual->nroFrame == nroFrame) return actual;
	}
	return NULL;
}

frame_entry* buscarFrameEntry(int nroPagina, int pid){
	int i;
	for(i=0;i<list_size(swapAdmin->framesEntries);i++){
		frame_entry* actual = list_get(swapAdmin->framesEntries, i);
		if(actual->pid == pid && actual->nroPagina == nroPagina) return actual;
	}
	return NULL;
}

t_list* buscarEntries(int pid){
	t_list* result = list_create();
	int i;
	for(i=0;i<list_size(swapAdmin->framesEntries);i++){
		frame_entry* actual = list_get(swapAdmin->framesEntries, i);
		if(actual->pid == pid) list_add(result, actual);
	}
	return result;
}
