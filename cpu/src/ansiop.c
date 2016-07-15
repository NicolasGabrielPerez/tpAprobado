#include "ansiop.h"

#include "cpu.h"

#include <commons/string.h>
#include <parser/metadata_program.h>


PCB* pcb;

t_variable* getMemoryAddr(t_puntero position) {

	log_trace(logger, "ANSISOP: getMemoryAddr, %d", position);
	t_variable* memoryAddr = malloc(sizeof(t_variable));
	memoryAddr->pageNumber = (position / PAGE_SIZE) + pcb->pageStart;
	memoryAddr->offset = position % PAGE_SIZE;
	memoryAddr->size = sizeof(t_valor_variable);

	return memoryAddr;
}

t_puntero definirVariable(t_nombre_variable variable) {

	log_trace(logger, "ANSISOP: definirVariable, %c", variable);

	//Reservar memoria, hay que enviar a la UMC?
	//umcDefine(variable);

	//TODO
	t_puntero memoryAddr = pcb->memoryIndex;
	pcb->memoryIndex++;

	char* key = string_from_format("%c", variable);

	t_puntero* value = malloc(sizeof(t_puntero));
	memcpy(value, &memoryAddr, sizeof(t_puntero));

	u_int32_t stackPosition = list_size(pcb->stack) - 1;
	t_stackContent* stackContent = list_get(pcb->stack, stackPosition);
	dictionary_put(stackContent->variables, key, value);

	free(key);

	return memoryAddr;
}



t_puntero obtenerPosicionVariable(t_nombre_variable variable) {

	log_trace(logger, "ANSISOP: obtenerPosicionVariable, %c", variable);

	char* key = string_from_format("%c", variable);

	u_int32_t stackPosition = list_size(pcb->stack) - 1;
	t_list* stack = pcb->stack;
	t_stackContent* stackContent = list_get(stack, stackPosition);
	t_puntero* memoryAddr = (t_puntero*) dictionary_get(stackContent->variables, key);

	free(key);

	return *memoryAddr;
}


t_valor_variable dereferenciar(t_puntero puntero) {

	log_trace(logger, "ANSISOP: dereferenciar, %d", puntero);

	t_variable* memoryAddr = getMemoryAddr(puntero);
	t_valor_variable value = umc_get(memoryAddr->pageNumber, memoryAddr->offset, memoryAddr->size);

	free(memoryAddr);

	return value;
}


void asignar(t_puntero puntero, t_valor_variable variable) {

	log_trace(logger, "ANSISOP: asignar Puntero: %d Valor: %d", puntero, variable);

	t_variable* memoryAddr = getMemoryAddr(puntero);

	char* param = string_itoa(variable);
	umc_set(memoryAddr->pageNumber, memoryAddr->offset, memoryAddr->size, param);
	free(param);
	free(memoryAddr);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida name) {

	log_trace(logger, "ANSISOP: obtenerValorCompartida, %c", name);

	//TODO: Testing

	t_valor_variable value = nucleo_variable_compartida_obtener(name);
	return value;
}


t_valor_variable asignarValorCompartida(t_nombre_compartida name, t_valor_variable value) {

	log_trace(logger, "ANSISOP: asignarValorCompartida, %c, %d", name, value);

	//TODO: Testing

	nucleo_variable_compartida_asignar(name, value);
	return value;
}


void irAlLabel(t_nombre_etiqueta etiqueta) {

	log_trace(logger, "ANSISOP: irAlLabel, %c", etiqueta);

	//TODO: Testing

	t_puntero_instruccion pointer = metadata_buscar_etiqueta(etiqueta, pcb->tagIndex, pcb->tagIndexSize);
	pcb->programCounter = pointer;
}

void callFunction(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {

	log_trace(logger, "ANSISOP: callFunction, Etiqueta: %s Donde Retornar: %d", etiqueta, donde_retornar);

	//TODO: Testing

	t_puntero_instruccion pointer = metadata_buscar_etiqueta(etiqueta, pcb->tagIndex, pcb->tagIndexSize);
	pcb->programCounter = pointer;

	t_list* stack = pcb->stack;

	//u_int32_t stackPosition = list_size(pcb->stack) - 1;
	//t_stackContent* currentStackContent = list_get(stack, stackPosition);

	//TODO: Hacer los argumentos de funcion al ser llamado.

	//Push
	t_stackContent* newStackContent = init_stackContent();
	newStackContent->returnAddress = donde_retornar;

	list_add(stack, newStackContent);
}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {

	log_trace(logger, "ANSISOP: llamarConRetorno, %c %d", etiqueta, donde_retornar);

	//TODO: Testing

	callFunction(etiqueta, donde_retornar);
}

void llamarSinRetorno(t_nombre_etiqueta etiqueta) {

	log_trace(logger, "ANSISOP: llamarSinRetorno, %s", etiqueta);

	//TODO: Testing

	callFunction(etiqueta, 0);
}

void finalizar() {
	log_trace(logger, "ANSISOP: finalizar");

	//TODO: Testing

	nucleo_notificarFinDePrograma(pcb);
}

void retornar(t_valor_variable valor) {

	log_trace(logger, "ANSISOP: retornar %d", valor);

	//Obtener contentido
	t_list* stack = pcb->stack;
	u_int32_t stackPosition = list_size(pcb->stack) - 1;
	t_stackContent* content = list_get(stack, stackPosition);

	pcb->programCounter = content->returnAddress;

	//Liberar del stack de variables en UMC, ver la manera de restar el index
	int size = dictionary_size(content->variables) * sizeof(t_valor_variable);
	pcb->memoryIndex -= size;

	t_puntero returnAddr = content->returnAddress;
	t_variable* returnVar = content->returnVariable;

	//Pop
	list_remove(stack, stackPosition);
	free_stackContent(content);

	pcb->programCounter = returnAddr;

	//TODO Revisar el retorno de la variable.
}

void imprimir(t_valor_variable valor) {

	log_trace(logger, "ANSISOP: imprimir %d", valor);

	//TODO: Testing

	nucleo_imprimir(valor);
}

void imprimirTexto(char* texto) {

	log_trace(logger, "ANSISOP: imprimirTexto, %c", texto);

	//TODO: Testing

	nucleo_imprimir_texto(texto);
}

void entradaSalida(t_nombre_dispositivo valor, u_int32_t tiempo) {

	log_trace(logger, "ANSISOP: entradaSalida %c, %d", valor, tiempo);

	//TODO: Testing

	nucleo_notificarIO(valor);
}


void wait(t_nombre_semaforo id) {

	log_trace(logger, "ANSISOP: wait %c", id);

	//TODO: Testing

	nucleo_wait(id);
}


void signal(t_nombre_semaforo id) {

	log_trace(logger, "ANSISOP: signal %c", id);

	//TODO: Testing

	nucleo_signal(id);
}
