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
#include "serialization.h"
#include "estructuras.h"

char* serializarInt(char* posicionDeEscritura, int32_t value){
	int inputSize = sizeof(int32_t);
	memcpy(posicionDeEscritura, value, inputSize);
	return posicionDeEscritura + inputSize;
}

int serializarString(char** serializado, char* value){
	memcpy(serializado, value, strlen(value));
	return strlen(serializado);
}

char* serializarPCB(PCB* pcb){
	int size = sizeOfPCB(pcb);
	char* serializado = malloc(size);

	serializado = serializarInt(serializado, pcb->identifier);
	serializado = serializarInt(serializado, pcb->programCounter);
	serializado = serializarInt(serializado, pcb->pageCode);

	appendIndexCode(serializado, &(pcb->indexCode));
	appendIndexTag(serializado, &(pcb->indexCode));

	serializado = serializarInt(serializado, pcb->indexStack);

	return serializado;
}

//El serializado viene con memoria YA RESERVADA
char* appendIndexCode(char* serializado, IndexCode* indexCode){
	serializado = serializarInt(serializado, indexCode->offsetStart);
	serializado = serializarInt(serializado, indexCode->offsetEnd);

	return serializado;
}

//El serializado viene con memoria YA RESERVADA
char* appendIndexTag(char* serializado, IndexTag* indexCode){

	return serializado;
}

int sizeOfPCB(PCB* pcb){
	int size = 0;
	size += sizeof(int32_t); //identifier;
	size += sizeof(int32_t); //programaCounter
	size += sizeof(int32_t); //pageCode
	size += sizeof(int32_t); //indexStack
	size += sizeOfIndexCode(pcb->indexCode);
	size += sizeOfIndexTag(pcb->indexTag);
	return size;
}

int sizeOfIndexCode(IndexCode* indexCode){
	int size = 0;
	size += sizeof(int32_t); //offsetStart
	size += sizeof(int32_t); //offsetEnd

	return size;
}

int sizeOfIndexTag(IndexTag* indexTag){
	int size = 0;
	return size;
}
