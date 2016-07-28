#include "ansiop.h"

#include "cpu.h"

#include <commons/string.h>
#include <parser/metadata_program.h>


PCB* pcb;

t_puntero definirVariable(t_nombre_variable variable) {

	log_trace(logger, "ANSISOP: definirVariable, %c", variable);

	t_puntero memoryAddr = pcb->memoryIndex + pcb->guti * PAGE_SIZE;;
	pcb->memoryIndex += sizeof(t_valor_variable);

	char* key = string_from_format("%c", variable);

	char* value = string_from_format("%d", memoryAddr);

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
	char* memoryAddrString = dictionary_get(stackContent->variables, key);

	t_puntero value = atoi(memoryAddrString);

	free(key);

	return value;
}


t_valor_variable dereferenciar(t_puntero puntero) {

	log_trace(logger, "ANSISOP: dereferenciar, %d", puntero);

//	t_variable* memoryAddr = getMemoryAddr(puntero);
//	char* buffer = umc_get(memoryAddr->pageNumber, memoryAddr->offset, memoryAddr->size);

	t_puntero  memoryAddr = puntero;

	u_int32_t page = memoryAddr / PAGE_SIZE;
	u_int32_t offset = memoryAddr % PAGE_SIZE;

	char* buffer = umc_get(page, offset, sizeof(t_valor_variable));

	if(buffer == 0) return 0;

	int32_t value = convertToInt32(buffer);

	free(buffer);

	return value;
}


void asignar(t_puntero puntero, t_valor_variable variable) {

	log_trace(logger, "ANSISOP: asignar Puntero: %d Valor: %d", puntero, variable);

	//t_variable* memoryAddr = getMemoryAddr(puntero);


	//umc_set(memoryAddr->pageNumber, memoryAddr->offset, memoryAddr->size, param);

	t_puntero  memoryAddr = puntero;

	char* intChar = malloc(sizeof(t_valor_variable));
	memcpy(intChar, &variable, sizeof(t_valor_variable));

	u_int32_t page = memoryAddr / PAGE_SIZE;
	u_int32_t offset = memoryAddr % PAGE_SIZE;

	umc_set(page, offset, sizeof(t_valor_variable), intChar);

	free(intChar);
	//free(memoryAddr);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida name) {

	log_trace(logger, "ANSISOP: obtenerValorCompartida, %s", name);

	char* blankedName = cleanStringSpaces(name);

	t_valor_variable value = nucleo_variable_compartida_obtener(blankedName);

	free(blankedName);

	return value;
}


t_valor_variable asignarValorCompartida(t_nombre_compartida name, t_valor_variable value) {

	log_trace(logger, "ANSISOP: asignarValorCompartida, %s, %d", name, value);

	char* blankedName = cleanStringSpaces(name);

	nucleo_variable_compartida_asignar(blankedName, value);

	free(blankedName);
	return value;
}


void irAlLabel(t_nombre_etiqueta etiqueta) {

	log_trace(logger, "ANSISOP: irAlLabel, %s", etiqueta);

	char* blankedEtiqueta = cleanStringSpaces(etiqueta);

	t_puntero_instruccion pointer = metadata_buscar_etiqueta(blankedEtiqueta, pcb->tagIndex, pcb->tagIndexSize);
	pcb->programCounter = pointer - 1;
}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {

	log_trace(logger, "ANSISOP: llamarConRetorno, %s %d", etiqueta, donde_retornar);

	t_list* stack = pcb->stack;

	t_stackContent* newStackContent = init_stackContent();
	newStackContent->returnAddress = pcb->programCounter;
	newStackContent->returnVariable = donde_retornar;

	pcb->memoryIndex += sizeof(t_valor_variable);

	char* blankedEtiqueta = cleanStringSpaces(etiqueta);

	t_puntero_instruccion pointer = metadata_buscar_etiqueta(blankedEtiqueta, pcb->tagIndex, pcb->tagIndexSize);
	pcb->programCounter = pointer - 1;

	list_add(stack, newStackContent);
}

void llamarSinRetorno(t_nombre_etiqueta etiqueta) {

	log_trace(logger, "ANSISOP: llamarSinRetorno, %s", etiqueta);

	//No hay que implementar nada
	t_puntero_instruccion pointer = metadata_buscar_etiqueta(etiqueta, pcb->tagIndex, pcb->tagIndexSize);
	pcb->programCounter = pointer;
}

void finalizar() {
	log_trace(logger, "ANSISOP: finalizar");

	nucleo_notificarFinDePrograma(pcb);
	hasToReturn = true;
	pcb = 0;
}

void retornar(t_valor_variable valor) {

	log_trace(logger, "ANSISOP: retornar %d", valor);

	//Obtener contentido
	t_list* stack = pcb->stack;
	u_int32_t stackPosition = list_size(pcb->stack) - 1;
	t_stackContent* content = list_get(stack, stackPosition);

	//Liberar del stack de variables en UMC, ver la manera de restar el index
	int size = dictionary_size(content->variables) * sizeof(t_valor_variable);
	pcb->memoryIndex -= size;

	t_puntero returnAddr = content->returnAddress;
	t_puntero returnVar = content->returnVariable;


	char* intChar = malloc(sizeof(t_valor_variable));
	memcpy(intChar, &valor, sizeof(t_valor_variable));

	u_int32_t page = returnVar / PAGE_SIZE;
	u_int32_t offset = returnVar % PAGE_SIZE;

	umc_set(page, offset, sizeof(t_valor_variable), intChar);

	//Pop
	list_remove(stack, stackPosition);
	free_stackContent(content);

	pcb->programCounter = returnAddr;
}

void imprimir(t_valor_variable valor) {

	log_trace(logger, "ANSISOP: imprimir %d", valor);

	//TODO: Testing

	nucleo_imprimir(valor);
}

void imprimirTexto(char* texto) {

	log_trace(logger, "ANSISOP: imprimirTexto, %s", texto);

	//TODO: Testing

	nucleo_imprimir_texto(texto);
}

void entradaSalida(t_nombre_dispositivo valor, u_int32_t tiempo) {

	log_trace(logger, "ANSISOP: entradaSalida %s, %d", valor, tiempo);

	//TODO: Testing

	//nucleo_notificarIO(valor);
}


void ansiop_wait(t_nombre_semaforo id) {

	log_trace(logger, "ANSISOP: wait %s", id);

	//TODO: Testing

	nucleo_wait(id);
}


void ansiop_signal(t_nombre_semaforo id) {

	log_trace(logger, "ANSISOP: signal %s", id);

	//TODO: Testing

	nucleo_signal(id);
}
