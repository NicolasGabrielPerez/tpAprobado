#ifndef DUMMY_ANSISOP_H_
	#define DUMMY_ANSISOP_H_

	#include <parser/parser.h>
	#include <stdio.h>



	t_puntero definirVariable(t_nombre_variable variable);
	t_puntero obtenerPosicionVariable(t_nombre_variable variable);
	t_valor_variable dereferenciar(t_puntero puntero);

	void asignar(t_puntero puntero, t_valor_variable variable);
	void imprimir(t_valor_variable valor);
	void imprimirTexto(char* texto);

#endif
