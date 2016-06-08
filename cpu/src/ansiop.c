#include "ansiop.h"
#include "nucleoFunctions.h"


t_puntero definirVariable(t_nombre_variable variable) {

	//Reservar memoria, hay que enviar a la UMC?
	//umcDefine(variable);

	t_puntero memoryAddr = 0xab;

	t_nombre_variable* key = malloc(sizeof(t_nombre_variable));
	memcpy(key, &variable, sizeof(t_nombre_variable));

	t_puntero* value = malloc(sizeof(t_puntero));
	memcpy(value, &memoryAddr, sizeof(t_puntero));

	//Agregar al pcb la variable
	t_list* stack = pcb->stack;
	StackContent* stackContent = list_get(stack, pcb->stackIndex);
	dictionary_put(stackContent->variables, key, value);

	printf("Definiendo variable %c\n", variable);
	return memoryAddr;
}


t_puntero obtenerPosicionVariable(t_nombre_variable variable) {


	t_list* stack = pcb->stack;
	StackContent* stackContent = list_get(stack, pcb->stackIndex);
	t_puntero* memoryAddr = (t_puntero*) dictionary_get(stackContent->variables, &variable);

	printf("Obtener posicion de %c\n", variable);
	return *memoryAddr;
}


t_valor_variable dereferenciar(t_puntero puntero) {


	//umcGet(puntero);

	t_valor_variable value = 5;

	printf("Dereferenciar %d y su valor es: %d\n", puntero, value);
	return value;
}


void asignar(t_puntero puntero, t_valor_variable variable) {

	//umcSet(puntero, variable);

	printf("Asignando en %d el valor %d\n", puntero, variable);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida valor) {


	return 0;
}


t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor) {

	return 0;
}


void irAlLabel(t_nombre_etiqueta etiqueta) {

}

void callFunction(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {
	t_list* stack = pcb->stack;

	//Get current stack content
	StackContent* currentStackContent = list_get(stack, pcb->stackIndex);

	//Push
	StackContent* newStackContent = init_stackContent();
	newStackContent->returnAddress = pcb->codeIndex;
	newStackContent->returnVariable = 0xff;

	pcb->stackIndex++;
	list_add(stack, newStackContent);
}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {

	callFunction(etiqueta, donde_retornar);
}

void llamarSinRetorno(t_nombre_etiqueta etiqueta) {
	callFunction(etiqueta, pcb->codeIndex);
}

void finalizar() {
	nucleo_notificarFinDePrograma(pcb);
}

void retornar(t_valor_variable valor) {

	//Obtener contentido
	t_list* stack = pcb->stack;
	StackContent* content = list_get(stack, pcb->stackIndex);

	//Asignar variable de retorno y program counter
	pcb->codeIndex = content->returnAddress;
	pcb->programCounter = 0;
	valor = content->returnVariable;

	//Pop
	free_stackContent(content);
	list_remove(stack, pcb->stackIndex);
	pcb->stackIndex--;
}

void imprimir(t_valor_variable valor) {
	printf("Imprimiendo: %d\n", valor);
}

void imprimirTexto(char* texto) {
	printf("Imprimiendo texto: %s\n", texto);
}

void entradaSalida(t_nombre_dispositivo valor, u_int32_t tiempo) {
	nucleo_notificarIO(valor, tiempo);
}


void wait(t_nombre_semaforo id) {
	nucleo_wait(id);
}


void signal(t_nombre_semaforo id) {
	nucleo_signal(id);
}
