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


t_puntero umcDefine(t_nombre_variable var);

t_valor_variable umcGet(t_puntero var);

void umcSet(t_puntero memoryAddr, t_nombre_variable var);

void umcRemove(t_puntero memoryAddr, t_nombre_variable var);


#endif
