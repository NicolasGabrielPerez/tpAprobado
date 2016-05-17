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
#ifndef UMC_LIBRARY_H_
#define UMC_LIBRARY_H_

#include <commons/collections/list.h>

typedef struct {
	t_list procesos;
} t_tlb;

typedef struct {
	char* memory;
} t_umc_main_memory;

typedef struct {
	char* memory;
} t_umc_cache_tlb;

/**
* @NAME: inicializarPrograma
* @DESC: Cuando el proceso Núcleo comunique el inicio
*  de un nuevo Programa AnSISOP, se crearán las estructuras
*   necesarias para administrarlo correctamente.
*   Además, deberá informar tal situación al Proceso Swap,
*   junto con la cantidad de páginas de datos a utilizar,
*   para que este asigne el espacio necesario en su partición.
*/
int inicializarPrograma(int program_id, int paginas_requeridas);


/**
* @NAME: solicitarBytesDePagina
* @DESC: Ante un pedido de lectura de página de alguno de los procesos CPU,
* se realizará la traducción a marco (frame) y se devolverá el contenido
* correspondiente. En caso de que la página no se encuentre en memoria principal,
*  será solicitada al proceso Swap, corriendo luego el algoritmo correspondiente
*  para cargarla en memoria principal.
*/
int solicitarBytesDePagina(int nro_pagina, int offset, int size);

/**
* @NAME: almacenarBytesEnPagina
* @DESC: Ante un pedido de escritura de página de alguno de los procesadores,
*  se realizará la traducción a marco (frame), y se actualizará su contenido.
*  En caso de que la página no se encuentre en memoria principal,
*  será solicitada al proceso Swap, corriendo luego el algoritmo correspondiente
*   para cargarla en memoria principal.
*   Es importante recordar que las escrituras a Swap se hacen únicamente
*   cuando se reemplaza una página que fue modificada previamente.
*/
int almacenarBytesEnPagina(int nro_pagina, int offset, int size, char* buffer);

/**
* @NAME: finalizarPrograma
* @DESC: Cuando el proceso Núcleo informe el fin de un Programa AnSISOP,
* se deberá eliminar las estructuras usadas para administrarlo.
* Además, deberá informar tal situación al proceso Swap,
* para que este libere el espacio utilizado en su partición
*/
int finalizarPrograma(int program_id);

/**
* @NAME: handshake
* @DESC: Tipo: Núcleo/CPU
*/
int handshake(int tipo);

/**
* @NAME: cambioDeProcesoActivo
*/
int cambioDeProcesoActivo(int program_id);

#endif /* UMC_LIBRARY_H_ */
