#include "ansiop.h"

#include "cpu.h"

#include <commons/string.h>
#include <parser/metadata_program.h>

t_variable* getMemoryAddr(t_puntero position) {

	log_trace(logger, string_from_format("ANSISOP: getMemoryAddr, %d", position));

	t_variable* memoryAddr = malloc(sizeof(t_variable));
	memoryAddr->pageNumber = (position / PAGE_SIZE) + pcb->pageStart;
	memoryAddr->offset = position % PAGE_SIZE;
	memoryAddr->size = sizeof(t_valor_variable);

	return memoryAddr;
}


t_puntero definirVariable(t_nombre_variable variable) {

	log_trace(logger, string_from_format("ANSISOP: definirVariable, %s", variable));

	//Reservar memoria, hay que enviar a la UMC?
	//umcDefine(variable);

	//TODO
	t_puntero memoryAddr = pcb->memoryIndex;
	pcb->memoryIndex++;


	t_nombre_variable* key = malloc(sizeof(t_nombre_variable));
	memcpy(key, &variable, sizeof(t_nombre_variable));

	t_puntero* value = malloc(sizeof(t_puntero));
	memcpy(value, &memoryAddr, sizeof(t_puntero));

	//Agregar al pcb la variable
	t_list* stack = pcb->stack;

	t_stackContent* stackContent = list_get(stack, pcb->stackPosition);
	dictionary_put(stackContent->variables, key, value);

	printf("Definiendo variable %c\n", variable);
	return memoryAddr;
}


t_puntero obtenerPosicionVariable(t_nombre_variable variable) {

	log_trace(logger, string_from_format("ANSISOP: obtenerPosicionVariable, %s", variable));

	t_list* stack = pcb->stack;
	t_stackContent* stackContent = list_get(stack, pcb->stackPosition);
	t_puntero* memoryAddr = (t_puntero*) dictionary_get(stackContent->variables, &variable);

	printf("Obtener posicion de %c\n", variable);
	return *memoryAddr;
}


t_valor_variable dereferenciar(t_puntero puntero) {

	log_trace(logger, string_from_format("ANSISOP: dereferenciar, %d", puntero));

	t_variable* memoryAddr = getMemoryAddr(puntero);
	t_valor_variable value = umc_get(memoryAddr->pageNumber, memoryAddr->offset, memoryAddr->size);

	free(memoryAddr);

	printf("Dereferenciar %d y su valor es: %d\n", puntero, value);
	return value;
}


void asignar(t_puntero puntero, t_valor_variable variable) {

	log_trace(logger, string_from_format("ANSISOP: asignar, %d, %d", puntero, variable));

	t_variable* memoryAddr = getMemoryAddr(puntero);

	char* param = string_itoa(variable);
	umc_set(memoryAddr->pageNumber, memoryAddr->offset, memoryAddr->size, param);
	free(param);
	free(memoryAddr);

	printf("Asignando en %d el valor %d\n", puntero, variable);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida name) {

	log_trace(logger, string_from_format("ANSISOP: obtenerValorCompartida, %s", name));

	t_valor_variable value = nucleo_variable_compartida_obtener(name);
	return value;
}


t_valor_variable asignarValorCompartida(t_nombre_compartida name, t_valor_variable value) {

	log_trace(logger, string_from_format("ANSISOP: asignarValorCompartida, %s, %d", name, value));

	nucleo_variable_compartida_asignar(name, value);
	return value;
}


void irAlLabel(t_nombre_etiqueta etiqueta) {

	log_trace(logger, string_from_format("ANSISOP: irAlLabel, %s", etiqueta));

	t_puntero_instruccion pointer = metadata_buscar_etiqueta(etiqueta, pcb->tagIndex, pcb->tagIndexSize);
	pcb->programCounter = pointer;
}

void callFunction(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {

	log_trace(logger, string_from_format("ANSISOP: callFunction, %s %d", etiqueta, retornar));

	t_puntero_instruccion pointer = metadata_buscar_etiqueta(etiqueta, pcb->tagIndex, pcb->tagIndexSize);
	pcb->programCounter = pointer;

	t_list* stack = pcb->stack;

	//Get current stack content
	//t_stackContent* currentStackContent = list_get(stack, pcb->stackPosition);

	//Push
	t_stackContent* newStackContent = init_stackContent();
	newStackContent->returnAddress = donde_retornar;

	pcb->stackIndex++;
	list_add(stack, newStackContent);
}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {

	log_trace(logger, string_from_format("ANSISOP: llamarConRetorno, %s %d", etiqueta, donde_retornar));


	callFunction(etiqueta, donde_retornar);
}

void llamarSinRetorno(t_nombre_etiqueta etiqueta) {

	log_trace(logger, string_from_format("ANSISOP: llamarSinRetorno, %s", etiqueta));

	callFunction(etiqueta, pcb->codeIndex);
}

void finalizar() {
	log_trace(logger, string_from_format("ANSISOP: finalizar"));

	nucleo_notificarFinDePrograma(pcb);
}

void retornar(t_valor_variable valor) {

	log_trace(logger, string_from_format("ANSISOP: retornar %d", valor));

	//Obtener contentido
	t_list* stack = pcb->stack;
	t_stackContent* content = list_get(stack, pcb->stackPosition);
	content->returnVariable = valor; //Setear ubicacion de la variable a retornar

	//Asignar variable de retorno y program counter
	pcb->codeIndex = content->returnAddress;

	//TODO
	pcb->programCounter = 0;
	valor = content->returnVariable;

	//Pop
	free_stackContent(content);
	list_remove(stack, pcb->stackIndex);
	pcb->stackIndex--;
}

void imprimir(t_valor_variable valor) {

	log_trace(logger, string_from_format("ANSISOP: imprimir %d", valor));

	nucleo_imprimir(valor);
}

void imprimirTexto(char* texto) {

	log_trace(logger, string_from_format("ANSISOP: imprimirTexto, %s", texto));

	nucleo_imprimir_texto(texto);
}

void entradaSalida(t_nombre_dispositivo valor, u_int32_t tiempo) {

	log_trace(logger, string_from_format("ANSISOP: entradaSalida %s, %d", valor, tiempo));

	nucleo_notificarIO(valor, tiempo);
}


void wait(t_nombre_semaforo id) {

	log_trace(logger, string_from_format("ANSISOP: wait %s", id));

	nucleo_wait(id);
}


void signal(t_nombre_semaforo id) {

	log_trace(logger, string_from_format("ANSISOP: signal %s", id));

	nucleo_signal(id);
}
