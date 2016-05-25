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

#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

#include <parser/parser.h>

#include "pcb.h"
#include "serialization.h"


PCB* init_pcb() {

	PCB* pcb = malloc(sizeof(PCB));
	pcb->indexStack = 0;
	pcb->stack = list_create();

	StackContent* stackContent = init_stackContent();
	list_add(pcb->stack, stackContent);

	return pcb;
}

void free_pcb(PCB* pcb) {

	int i = 0;
	for(i = 0; i <= pcb->indexStack; i++) {
		StackContent* stackContent = list_get(pcb->stack, i);
		free_stackContent(stackContent);
	}

	list_destroy(pcb->stack);
	free(pcb);
}

StackContent* init_stackContent() {

	StackContent* stackContent = malloc(sizeof(StackContent));
	stackContent->variables = dictionary_create();
	stackContent->arguments = dictionary_create();
	stackContent->returnAddress = 3;
	stackContent->returnVariable = 2;

	return stackContent;
}

void free_stackContent(StackContent* stackContent) {

	dictionary_destroy_and_destroy_elements(stackContent->arguments, free);
	dictionary_destroy_and_destroy_elements(stackContent->variables, free);
	free(stackContent);
}

//Lo que viene lo saque de aca pero no tuve tiempo de implementarlo
//http://stackoverflow.com/questions/6002528/c-serialization-techniques
void serialize_int(int x, Buffer *b) {
    /* assume int == long; how can this be done better? */
    //x = htonl(x);

    //reserve_space(b, sizeof(int));

    //memcpy(((char *)b->data) + b->next, &x, sizeof(int));
    //b->next += sizeof(int);
}

void serialize_string(char* string, Buffer *b) {
    /* assume int == long; how can this be done better? */

	//x = htonl(string);

    //reserve_space(b, sizeof(int));

    //memcpy(((char *)b->data) + b->next, &x, sizeof(int));
    //b->next += sizeof(int);
}

void serialize_pcb(struct PCB *x, struct Buffer *output) {
    //serialize_int(x->arguments, output);
    //serialize_int(x->variables, output);
    //serialize_string(x->string, output);
}


