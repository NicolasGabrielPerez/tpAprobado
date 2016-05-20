#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <parser/parser.h>
#include "commons/collections/list.h"

#include "pcb.h"


//CPU a UMC
typedef struct cpu_to_umc {

	//Fijate si necesitas el PCB entero o si solo necesitas el processId.
	//PCB* pcb;
	//int processID;

	char* action;  //accion, puede ser define, set, get o remove
	t_puntero memoryAddr;
	t_nombre_variable value;
} cpu_to_umc;

//Inicializa una variable y debe devolver la posicion de memoria.
t_puntero cpu_to_umc_define(t_valor_variable var);

//Obtiene el valor de una variable a partir de la posicion de memoria.
t_valor_variable cpu_to_umc_get(t_puntero var);

//Guarda una variable en memoryAddr con el valor var.
void cpu_to_umc_set(t_puntero memoryAddr, t_valor_variable var);

//Elimina una posicion de memoria
void cpu_to_umc_remove(t_puntero memoryAddr);




//NUCLEO a CPU
typedef struct nucleo_to_cpu {

	PCB* pcb;
	t_list list; //array de char* de las instrucciones que hay que ejecutar.
} nucleo_to_cpu;


#endif
