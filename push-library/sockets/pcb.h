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
#include <commons/collections/queue.h>

#include <parser/parser.h>

extern t_list* General_Process_List;

//--------------------------Structs
typedef struct Variable{
	char* id;
	t_puntero position;
} t_variable;

typedef struct stackContent {
	t_dictionary* arguments;
	t_dictionary* variables;
	t_puntero returnAddress;
	u_int32_t returnVariable;
} t_stackContent;

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
	u_int32_t tagIndexSize;			// Tamaño del mapa serializado de etiquetas
	t_list* stack;
	u_int32_t stackCount;
	t_puntero memoryIndex;
	u_int32_t guti;					//Nro de página relativa en donde empieza el stack


} PCB;
//--------------------------Structs

//--------------------------Functions
PCB* new_pcb(int processID);

void free_pcb(PCB* pcb);

t_stackContent* init_stackContent();

void free_stackContent(t_stackContent* stackContent);

void create_program_PCB(PCB* pcb, char* program, int codePagesCount);

t_link_element* list_find_by_id(t_list *self, int ID, bool(*condition)(void*, int), int* index);

PCB* pcb_list_find_element(t_list *self, int PID, bool(*condition)(void*, int));

int find_pcb(PCB* pcb, int PID);

PCB* get_pcb_by_ID(t_list* pcbList, int PID);

PCB* remove_pcb_by_ID(t_list* pcbList, int PID);

int get_next_Process_ID();
//--------------------------Functions

#endif
