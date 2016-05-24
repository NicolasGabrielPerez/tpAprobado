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
	StackContent* stackContent = list_get(stack, pcb->indexStack);
	dictionary_put(stackContent->arguments, key, value);

	printf("Definiendo variable %c\n", variable);
	return memoryAddr;
}


t_puntero obtenerPosicionVariable(t_nombre_variable variable) {


	t_list* stack = pcb->stack;
	StackContent* stackContent = list_get(stack, pcb->indexStack);
	t_puntero* memoryAddr = (t_puntero*) dictionary_get(stackContent->arguments, &variable);

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


void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {

	//Push
	StackContent* content; //= stack_content_create();
	t_list* stack = pcb->stack;
	pcb->indexStack++;
	list_add(stack, content);
}

void llamarSinRetorno(t_nombre_etiqueta etiqueta) {

	//Push
	StackContent* content; //= stack_content_create();
	t_list* stack = pcb->stack;
	pcb->indexStack++;
	list_add(stack, content);
}

void finalizar() {

	//Informar fin de programa al nucleo

}

void retornar(t_valor_variable valor) {

	//Pop
	t_list* stack = pcb->stack;
	StackContent* content = list_get(stack, pcb->indexStack);
	pcb->indexStack--;

	free_stackContent(content);
}

void imprimir(t_valor_variable valor) {
	printf("Imprimiendo: %d\n", valor);
}


void imprimirTexto(char* texto) {
	printf("Imprimiendo texto: %s\n", texto);
}

void entradaSalida(t_nombre_dispositivo valor, int tiempo) {


}


void wait(t_nombre_semaforo id) {
	nucleo_wait(id);
}


void signal(t_nombre_semaforo id) {
	nucleo_signal(id);
}
