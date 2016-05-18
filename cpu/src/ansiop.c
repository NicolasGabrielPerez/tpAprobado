#include "ansiop.h"


static const int CONTENIDO_VARIABLE = 20;
static const int POSICION_MEMORIA = 0x10;


t_puntero definirVariable(t_nombre_variable variable) {

	//Reservar memoria, hay que enviar a la UMC?

	//Agregar al stack la variable
	//Falta PCB


	//retornar posicion de memoria de la nueva variable


	printf("definir la variable %c\n", variable);
	return POSICION_MEMORIA;
}


t_puntero obtenerPosicionVariable(t_nombre_variable variable) {

	//Devuelve posicion de memoria
	//Falta PCB

	printf("Obtener posicion de %c\n", variable);
	return POSICION_MEMORIA;
}


t_valor_variable dereferenciar(t_puntero puntero) {
	printf("Dereferenciar %d y su valor es: %d\n", puntero, CONTENIDO_VARIABLE);
	return CONTENIDO_VARIABLE;
}


void asignar(t_puntero puntero, t_valor_variable variable) {
	printf("Asignando en %d el valor %d\n", puntero, variable);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida valor) {
	printf("Imprimir %d\n", valor);

	return 0;
}


t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor) {
	printf("Imprimir %d\n", valor);

	return 0;
}


void irAlLabel(t_nombre_etiqueta etiqueta) {
	printf("Imprimir %d\n", etiqueta);
}


void llamarConRetorno(t_nombre_etiqueta etiqueta) {
	printf("Imprimir %d\n", etiqueta);
}

void llamarSinRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {

}

void finalizar() {
	printf("Imprimir ");
}

void retornar(t_valor_variable valor) {
	printf("Imprimir %d\n", valor);
}

void imprimir(t_valor_variable valor) {
	printf("Imprimir %d\n", valor);
}


void imprimirTexto(char* texto) {
	printf("ImprimirTexto: %s", texto);
}

void entradaSalida(t_nombre_dispositivo valor, int tiempo) {
	printf("Imprimir %d\n", valor);
}


void wait(t_nombre_semaforo id) {

}


void signal(t_nombre_semaforo id) {

}
