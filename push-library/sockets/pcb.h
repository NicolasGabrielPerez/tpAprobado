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
#include <parser/metadata_program.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

#include <parser/parser.h>
#define PCB_H_

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
	int offsetStart;
	int offsetEnd;
} IndexCode;

typedef struct PCB {
	int processId;					//Identificador único del proceso
	int programCounter;				//Nro. de la próxima instrucción a ejecutar
	int codePagesCount;				//Cantidad de páginas de memoria asignadas al código
	t_intructions* codeIndex;		//Índice de código
	t_size instructionsCount;		//Cantidad de instrucciones del programa
	char* tagIndex;					//Índice de etiquetas, concatenado en una única cadena
	int tagIndexSize;				// Tamaño del mapa serializado de etiquetas
	StackContent** stackIndex;
	t_list* stack;

} PCB;


PCB* init_pcb();
void free_pcb(PCB* pcb);

StackContent* init_stackContent();
void free_stackContent(StackContent* stackContent);

void create_program_PCB(PCB* pcb, char* program);

#endif
