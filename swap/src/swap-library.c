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

#include "swap-library.h"

t_swap_block* init_swap(int size){
	char* memory_block = malloc(size);
	if(memory_block == 0){
		return NULL;
	}

	t_swap_block* swap = malloc(sizeof(t_swap_block));

	swap->memory_block = memory_block;
	swap->size = size;
	swap->disponible = size;
	return swap;
}
