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

t_list* General_Process_List;

int get_greater_value(PCB* pcb, int lastId){
	if(pcb->processId > lastId)
		return pcb->processId;
	else
		return lastId;
}

int get_next_Process_ID(){
	int lastId = 0;

	if(list_size(General_Process_List) > 0){
		t_link_element *element = General_Process_List->head;
		while (element != NULL) {
			lastId = get_greater_value(element->data, lastId);
			element = element->next;
		}
		lastId++;
		return (lastId);
	}
	else
		return 0;

}

void create_program_PCB(PCB* pcb, char* program, int codePagesCount){
	t_metadata_program *programMetadata = malloc(sizeof(t_metadata_program));
	programMetadata = metadata_desde_literal(program);

	pcb->programCounter = programMetadata->instruccion_inicio;
	pcb->tagIndex = programMetadata->etiquetas;
	pcb->tagIndexSize = programMetadata->etiquetas_size;
	pcb->instructionsCount = programMetadata->instrucciones_size;
	pcb->codeIndex = programMetadata->instrucciones_serializado;
	pcb->codePagesCount = codePagesCount;
	pcb->stackPosition = 0;
}

PCB* new_pcb(int processID) {

	PCB* pcb = malloc(sizeof(PCB));
	pcb->processId = processID;
	pcb->stackIndex = list_create();

	return pcb;
}

//TODO:Guti-Implementar
void free_pcb(PCB* pcb) {

	//int i = 0;
	//for(i = 0; i < pcb->indexStack; i++) {
	//	StackContent* stackContent = list_get(pcb->stack, i);
	//	free_stackContent(stackContent);
	//}

	list_destroy(pcb->stackIndex);
	free(pcb);
}

t_stackContent* init_stackContent() {

	t_stackContent* stackContent = malloc(sizeof(t_stackContent));
	stackContent->arguments = dictionary_create();
	stackContent->variables = dictionary_create();

	return stackContent;
}

void free_stackContent(t_stackContent* stackContent) {

	dictionary_destroy(stackContent->arguments);
	free(stackContent);
}

//Redefinición de método para buscar dentro de una lista
t_link_element* list_find_by_id(t_list *self, int ID, bool(*condition)(void*, int), int* index) {
	t_link_element *element = self->head;
	int position = 0;

	while (element != NULL && !condition(element->data, ID)) {
		element = element->next;
		position++;
	}

	if (index != NULL) {
		*index = position;
	}

	return element;
}

//Redefinición de método. Devuelve un PCB según PID
PCB* pcb_list_find_element(t_list *self, int PID, bool(*condition)(void*, int)) {
	t_link_element *element = list_find_by_id(self, PID,condition, NULL);
	return element != NULL ? element->data : NULL;
}

int find_pcb(PCB* pcb, int PID){
	return pcb->processId == PID;
}

PCB* get_pcb_by_ID(t_list* pcbList, int PID){
	return pcb_list_find_element(pcbList, PID, find_pcb);
}

PCB* remove_pcb_by_ID(t_list* pcbList, int PID){
	int index = 0;

	t_link_element* element = list_find_by_id(pcbList, PID, find_pcb, &index);
	if (element != NULL) {
		return list_remove(pcbList, index);
	}

	return NULL;
}
