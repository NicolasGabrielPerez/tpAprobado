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
#ifndef UMC_FUNCTIONS_H_
#define UMC_FUNCTIONS_H_

#include <parser/parser.h>
#include <commons/config.h>

u_int32_t PAGE_SIZE;

void umc_init(t_config* config);
void umc_delete();


void umc_process_active(int processId);
void umc_set(t_puntero page, t_puntero offset, t_size size, char* buffer);
char* umc_get(t_puntero page, t_puntero offset, t_size size);


#endif
