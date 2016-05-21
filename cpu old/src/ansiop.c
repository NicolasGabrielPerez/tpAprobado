#include "ansiop.h"

t_puntero definirVariable(t_nombre_variable variable) {

	//Reservar memoria, hay que enviar a la UMC?
	//umcDefine(variable);

	t_puntero memoryAddr = 0xab;

	//Agregar al pcb la variable
	t_list* stack = pcb->stack;
	StackContent* stackContent = list_get(stack, pcb->indexStack);
	dictionary_put(stackContent->arguments, &variable, &memoryAddr);

	printf("Definiendo variable %c\n", variable);
	return memoryAddr;
}


t_puntero obtenerPosicionVariable(t_nombre_variable variable) {


	t_list* stack = pcb->stack;
	StackContent* stackContent = list_get(stack, pcb->indexStack);
	t_puntero memoryAddr = (int) dictionary_get(stackContent->arguments, &variable);

	printf("Obtener posicion de %c\n", variable);
	return memoryAddr;
}


t_valor_variable dereferenciar(t_puntero puntero) {


	//umcGet(puntero);

	t_valor_variable value = 5;

	printf("Dereferenciar %d y su valor es: %d\n", puntero, value);
	return value;
}


void asignar(t_puntero puntero, t_valor_variable variable) {

	//umcSet(puntero, variable);

	t_list* stack = pcb->stack;
	StackContent* stackContent = list_get(stack, pcb->indexStack);
	dictionary_put(stackContent->arguments, &variable, &puntero);

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


void llamarConRetorno(t_nombre_etiqueta etiqueta) {

	//Push
	StackContent* content; //= stack_content_create();
	t_list* stack = pcb->stack;
	pcb->indexStack++;
	list_add(stack, content);
}

void llamarSinRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {

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

	//stack_content_free(content);
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
	wait(id);
}


void signal(t_nombre_semaforo id) {
	signal(id);
}
