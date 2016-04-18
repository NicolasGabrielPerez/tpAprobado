/*
 ============================================================================
 Name        : librery.h
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description :
 ============================================================================
 */

#ifndef CONEXION_CON
#define CONEXION_CON

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int crear_cliente(int* socketCliente,char* IP,char* PUERTO);

int crear_servidor(int* listenningSocket, char* PUERTO);



#endif

