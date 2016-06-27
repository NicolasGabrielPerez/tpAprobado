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
#ifndef PCB_H_
#define PCB_H_
#include <parser/metadata_program.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

#include <parser/parser.h>


typedef struct Variable{
	char* id;
	int32_t pageNumber;
	int32_t offset;
	t_size size;
} t_variable;

typedef struct stackIndex {
	int32_t instructionNumber;	//Número de instrucción dentro del código
	t_dictionary* arguments; 	//Diccionario de argumentos, en el caso de hallarse una función
	t_variable* variables; 		//Lista de variables
	t_puntero returnAddress;	//
	t_puntero returnVariable;
} t_stackIndex;


typedef struct stackContent {
	t_dictionary* arguments; //Diccionario
	t_dictionary* variables; //Lista
	t_puntero returnAddress;
	t_variable returnVariable;
} StackContent;

typedef struct indexTag {

//Estructura auxiliar utilizada para conocer las líneas de
//código correspondientes al inicio de los procedimientos
//y a las etiquetas. Esto que seria exactamente?

} IndexTag;

typedef struct indexCode {
	u_int32_t offsetStart;
	u_int32_t offsetEnd;
} IndexCode;

typedef struct PCB {
	u_int32_t processId;					//Identificador único del proceso
	u_int32_t programCounter;				//Nro. de la próxima instrucción a ejecutar
	u_int32_t codePagesCount;				//Cantidad de páginas de memoria asignadas al código
	t_intructions* codeIndex;		//Índice de código
	t_size instructionsCount;		//Cantidad de instrucciones del programa
	char* tagIndex;					//Índice de etiquetas, concatenado en una única cadena
	u_int32_t tagIndexSize;				// Tamaño del mapa serializado de etiquetas
	StackContent** stackIndex;//Agus: Chicos, a que le llaman index? No se olviden que INDEX es un NUMERO
	t_list* stack;
	u_int32_t stackCount; //Esto lo agregue yo (Agus), si me quitan el numero de elementos del stack no tengo forma de saber cual es el ultimo elemento!

	//Agregado Agus, creo que es necesario para la memoria, dsp lo vemos
	u_int32_t pageStart;
	u_int32_t pagesCount;
	t_puntero memoryIndex;

} PCB;


PCB* init_pcb();
void free_pcb(PCB* pcb);

StackContent* init_stackContent();
void free_stackContent(StackContent* stackContent);

void create_program_PCB(PCB* pcb, char* program);

#endif
