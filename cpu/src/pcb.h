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

#include <commons/collections/list.h>

typedef struct stackContent {

	t_dictionary* arguments; //Argumentos de la funcion
	t_dictionary* variables; //Variables locales
	t_puntero returnAddress;
	t_puntero returnVariable;
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


typedef struct pcb {
	int identifier;
	int programCounter;
	int pageCount;
	IndexCode* indexCode;
	IndexTag* indexTag;
	int indexStack;
	t_list* stack;

} PCB;


PCB* init_pcb();
void free_pcb(PCB* pcb);

StackContent* init_stackContent();
void free_stackContent(StackContent* stackContent);

#endif
