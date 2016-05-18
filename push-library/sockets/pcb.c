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


typedef struct StackContent {

	t_dictionary arguments; //Diccionario
	t_list variables; //Lista
	t_puntero returnAddress;
	t_puntero returnVariable;
};

typedef struct IndexTag {

//Estructura auxiliar utilizada para conocer las líneas de
//código correspondientes al inicio de los procedimientos
//y a las etiquetas. Esto que seria exactamente?

} IndexTag;

typedef struct IndexCode {
	int offsetStart;
	int offsetEnd;
} IndexCode;


typedef struct PCB {
	int identifier;
	int programCounter;
	int pageCode;
	IndexCode indexCode;
	IndexTag indexTag;
	int indexStack;

} PCB;



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


