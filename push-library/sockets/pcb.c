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

#include "pcb.h"
#include "serialization.h"

void create_program_PCB(PCB* pcb, char* program){
	t_metadata_program *programMetadata = malloc(sizeof(t_metadata_program));
	programMetadata = metadata_desde_literal(program);

	pcb->processId = 0;			//TODO: inicializar correctamente
	pcb->programCounter = programMetadata->instruccion_inicio;
	pcb->tagIndex = programMetadata->etiquetas;
	pcb->tagIndexSize = programMetadata->etiquetas_size;
	pcb->instructionsCount = programMetadata->instrucciones_size;
	pcb->codeIndex = programMetadata->instrucciones_serializado;
	pcb->codePagesCount = 0;		//TODO: Asignar correctamente la cantidad de páginas de memoria
	pcb->stack = NULL;
	pcb->stackIndex = NULL;
}


PCB* init_pcb() {

	PCB* pcb = malloc(sizeof(PCB));
	//pcb->indexStack = 0;
	pcb->stack = list_create();

	StackContent* stackContent = init_stackContent();
	list_add(pcb->stack, stackContent);

	return pcb;
}

void free_pcb(PCB* pcb) {

	int i = 0;
	//for(i = 0; i < pcb->indexStack; i++) {
	//	StackContent* stackContent = list_get(pcb->stack, i);
	//	free_stackContent(stackContent);
	//}

	list_destroy(pcb->stack);
	free(pcb);
}

StackContent* init_stackContent() {

	StackContent* stackContent = malloc(sizeof(StackContent));
	stackContent->arguments = dictionary_create();

	return stackContent;
}

void free_stackContent(StackContent* stackContent) {

	dictionary_destroy(stackContent->arguments);
	free(stackContent);
}


