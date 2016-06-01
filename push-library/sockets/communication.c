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

#include "communication.h"

int32_t HEADER_SIZE = sizeof(int32_t);
int32_t RESPUESTA_SIZE = sizeof(int32_t);
int32_t RESPUESTA_OK = 10;
int32_t RESPUESTA_FAIL = -10;

int32_t HEADER_HANDSHAKE = 100;
int32_t HEADER_INIT_PROGRAMA = 200;
int32_t HEADER_SOLICITAR_PAGINAS = 300;
int32_t HEADER_ALMACENAR_PAGINAS = 400;
int32_t HEADER_FIN_PROGRAMA = 600;
int32_t HEADER_CAMBIO_PROCESO_ACTIVO = 700;

int32_t TIPO_CONSOLA = 1000;
int32_t TIPO_NUCLEO = 2000;
int32_t TIPO_UMC = 3000;
int32_t TIPO_SWAP = 4000;
int32_t TIPO_CPU = 5000;
int32_t TIPO_SIZE = sizeof(int32_t);
