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

//El serializado viene con memoria YA RESERVADA
char* appendIndexCode(char* serializado, IndexCode* indexCode){
	serializado = serializarInt(serializado, indexCode->offsetStart);
	serializado = serializarInt(serializado, indexCode->offsetEnd);

	return serializado;
}

//El serializado viene con memoria YA RESERVADA
char* appendIndexTag(char* serializado, IndexTag* indexTag){
	serializado = serializarInt(serializado, indexTag->data);
	return serializado;
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

int sizeOfPCB(PCB* pcb){
	int size = 0;
	size += sizeof(int32_t); //identifier;
	size += sizeof(int32_t); //programaCounter
	size += sizeof(int32_t); //pageCode
	size += sizeof(int32_t); //indexStack
	size += sizeOfIndexCode(&pcb->indexCode);
	size += sizeOfIndexTag(&pcb->indexTag);
	return size;
}



char* serializarInt(char* posicionDeEscritura, int32_t value){
	int inputSize = sizeof(int32_t);
	memcpy(posicionDeEscritura, value, inputSize);
	return posicionDeEscritura + inputSize;
}

char* serializarString(char* posicionDeEscritura, char* value){
	int inputSize = strlen(value) + 1;
	memcpy(posicionDeEscritura, value, inputSize);
	return posicionDeEscritura + inputSize;
}

char* serializarPCB(PCB* pcb){
	int size = sizeOfPCB(pcb);
	char* serializado = malloc(size);

	serializado = serializarInt(serializado, pcb->identifier);
	serializado = serializarInt(serializado, pcb->programCounter);
	serializado = serializarInt(serializado, pcb->pageCode);

	serializado = appendIndexCode(serializado, &pcb->indexCode);
	serializado = appendIndexTag(serializado, &pcb->indexTag);

	serializado = serializarInt(serializado, pcb->indexStack);

	return serializado;
}

char* deserializarIndexCodeEnPCB(PCB* pcb, char* serializado){
	char* siguiente = serializado;

	memcpy(pcb->indexCode.offsetStart, siguiente, sizeof(int32_t));
	siguiente += sizeof(int32_t);

	memcpy(pcb->indexCode.offsetEnd, siguiente, sizeof(int32_t));
	siguiente += sizeof(int32_t);

	return siguiente;
}

char* deserializarIndexTagEnPCB(PCB* pcb, char* serializado){
	char* siguiente = serializado;

	memcpy(pcb->indexTag.data, siguiente, sizeof(int32_t));
	siguiente += sizeof(int32_t);

	return siguiente;
}

PCB* deserializarPCB(char* serializado){
	PCB* pcb = malloc(sizeof(PCB));
	char* siguiente = serializado;

	memcpy(&pcb->identifier, siguiente, sizeof(int32_t));
	siguiente += sizeof(int32_t);

	memcpy(&pcb->programCounter, siguiente, sizeof(int32_t));
	siguiente += sizeof(int32_t);

	memcpy(&pcb->pageCode, siguiente, sizeof(int32_t));
	siguiente += sizeof(int32_t);

	siguiente = deserializarIndexCodeEnPCB(pcb, serializado);

	siguiente = deserializarIndexTagEnPCB(pcb, serializado);

	memcpy(&pcb->indexStack, siguiente, sizeof(int32_t));

	return pcb;
}


