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

#include "serialization.h"

#define INITIAL_SIZE 32

struct Buffer *new_buffer() {
    struct Buffer *b = malloc(sizeof(Buffer));

    b->data = malloc(INITIAL_SIZE);
    b->size = INITIAL_SIZE;
    b->next = 0;

    return b;
}

void reserve_space(Buffer *b, size_t bytes) {
    if((b->next + bytes) > b->size) {
        /* double size to enforce O(lg N) reallocs */
        b->data = realloc(b->data, b->size * 2);
        b->size *= 2;
    }
}
