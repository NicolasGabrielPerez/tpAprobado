#ifndef ANSISOP_H
	#define ANSISOP_H


	#include <stdio.h>

	#include "commons/collections/list.h"
	#include "commons/collections/dictionary.h"
	#include <parser/parser.h>


	#include <sockets/pcb.h>
	#include "nucleoFunctions.h"
	#include "umcFunctions.h"



	extern PCB* pcb;

	t_puntero definirVariable(t_nombre_variable variable);
	t_puntero obtenerPosicionVariable(t_nombre_variable variable);
	t_valor_variable dereferenciar(t_puntero puntero) ;
	void asignar(t_puntero puntero, t_valor_variable variable);

	t_valor_variable obtenerValorCompartida(t_nombre_compartida valor);
	t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor);

	void irAlLabel(t_nombre_etiqueta etiqueta);
	void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar);
	void llamarSinRetorno(t_nombre_etiqueta etiqueta);

	void finalizar();
	void retornar(t_valor_variable valor);

	void imprimir(t_valor_variable valor);
	void imprimirTexto(char* texto);

	void entradaSalida(t_nombre_dispositivo valor, u_int32_t tiempo);

	void ansiop_wait(t_nombre_semaforo id);
	void ansiop_signal(t_nombre_semaforo id);


#endif
