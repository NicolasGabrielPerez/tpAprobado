#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/string.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sockets/sockets.h>
#include <pthread.h>
#include "umc-structs.h"

int32_t HEADER_HANDSHAKE = 100;
int32_t HEADER_INIT_PROGRAMA = 200;
int32_t HEADER_SOLICITAR_PAGINAS = 300;
int32_t HEADER_ALMACENAR_PAGINAS = 400;
int32_t HEADER_CAMBIO_PROCESO_ACTIVO = 500;
int32_t HEADER_FIN_PROGRAMA = 600;

int32_t HEADER_SIZE = sizeof(int32_t);

#define TIPO_NUCLEO 1
#define TIPO_CPU 2
#define TIPO_SWAP 3
#define PEDIDO_INIT_PROGRAMA 1
#define PEDIDO_FINALIZAR_PROGRAMA 2
#define PEDIDO_LECTURA 1

int32_t RESPUESTA_OK = 10;
int32_t RESPUESTA_FAIL = -10;

int RESPUESTA_SIZE = sizeof(int32_t);

int listener;
int swap_socket;
int cantidad_de_marcos;

int* TLBEnable = 0;
tlb* TLB;

tabla_de_frames* tablaDeFrames;
char* memoria_bloque;

t_list* tablasDePaginas;

pthread_attr_t attr;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int marco_size; //cant bytes
int marcos_x_proc; //cant paginas

char* initProgramaSwap(int* pid, int* cantPaginas, char* codFuente){
	if (send(swap_socket, &HEADER_INIT_PROGRAMA, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(swap_socket, pid, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(swap_socket, cantPaginas, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(swap_socket, codFuente, (*cantPaginas)*marco_size, 0) == -1) {
		perror("send");
		exit(1);
	}
	char* respuestaSwap = malloc(RESPUESTA_SIZE);
	if (recv(swap_socket, respuestaSwap, RESPUESTA_SIZE, 0) == -1) {
		perror("recv");
		exit(1);
	}
	return respuestaSwap;
}

char* finalizarProgramaSwap(int* pid){
	if (send(swap_socket, &HEADER_FIN_PROGRAMA, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	if (send(swap_socket, pid, sizeof(int32_t), 0) == -1) {
		perror("send");
		exit(1);
	}
	char* respuestaSwap = malloc(RESPUESTA_SIZE);
	if (recv(swap_socket, respuestaSwap, RESPUESTA_SIZE, 0) == -1) {
		perror("recv");
		exit(1);
	}
	return respuestaSwap;
}

tabla_de_paginas* buscarPorPID(t_list* tablas, int pidABuscar){
	int i;
	for(i=0; i<list_size(tablas);i++){
		tabla_de_paginas* actual = list_get(tablas, i);
		printf("pid actual:%d\n", actual->pid);
		if(actual->pid == pidABuscar){
			return actual;
		}
	}
	return NULL;
};

tabla_de_paginas_entry* buscarEntradaPorPagina(tabla_de_paginas* tablaDePaginas, int nroPagina){
	int i;
	for(i=0; i<list_size(tablaDePaginas->entradas);i++){
		tabla_de_paginas_entry* actual = list_get(tablaDePaginas->entradas, i);
		printf("nroPagina actual:%d\n", actual->nroPagina);
		if(actual->nroPagina == nroPagina){
			return actual;
		}
	}
	return NULL;
};

char* initProgramaUMC(int pid, int cantPaginas){
	// verificar que no exista pid
	if(buscarPorPID(tablasDePaginas, pid)!=NULL){
		return string_itoa(RESPUESTA_FAIL);
	}
	// verificar que cantPaginas + stackSize < MAX
	if(cantPaginas > marcos_x_proc){
		return string_itoa(RESPUESTA_FAIL);
	}
	// crear tabla de paginas
	tabla_de_paginas* tablaDePaginas = malloc(sizeof(tabla_de_paginas));
	tablaDePaginas->entradas = list_create();
	tablaDePaginas->pid = pid;

	list_add(tablasDePaginas, tablaDePaginas);
	return string_itoa(RESPUESTA_OK);
}

char* recbirInitPrograma(int nucleo_socket){
	int bytes_recibidos;
	int32_t pid;
	int32_t cantPaginas;
	int codFuente_size;
	char* codFuente;
	char* respuesta;

	if ((bytes_recibidos = recv(nucleo_socket, &pid, sizeof(int32_t), 0)) == -1) {
		perror("recv");
		exit(1);
	}
	if ((bytes_recibidos = recv(nucleo_socket, &cantPaginas, sizeof(int32_t), 0)) == -1) {
		perror("recv");
		exit(1);
	}
	codFuente_size = cantPaginas*marco_size;
	codFuente = malloc(codFuente_size);
	if ((bytes_recibidos = recv(nucleo_socket, codFuente, codFuente_size, 0)) == -1) {
		perror("recv");
		exit(1);
	}

	respuesta = initProgramaSwap(&pid, &cantPaginas, codFuente);

	int32_t respuestaInt;
	memcpy(&respuestaInt, respuesta, RESPUESTA_SIZE);

	if(respuestaInt == RESPUESTA_OK){
		respuesta = initProgramaUMC(pid, cantPaginas);
	} else {
		respuesta = string_itoa(RESPUESTA_FAIL);
	}

	free(codFuente);

	if (send(nucleo_socket, respuesta, RESPUESTA_SIZE, 0) == -1) {
		perror("send");
		exit(1);
	}

	return 0;
}

char* recibirFinalizarPrograma(int nucleo_socket){
	int32_t pid;
	char* respuesta;

	if (recv(nucleo_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	respuesta = finalizarProgramaSwap(&pid);

	if (send(nucleo_socket, respuesta, RESPUESTA_SIZE, 0) == -1) {
		perror("send");
		exit(1);
	}

	return 0;

}

char* pedirPaginaASwap(int nroPagina, int pid){
	return 0;
}

char* obtenerBytes(char* contenido, int offset, int tamanio){
	char* bytes = malloc(tamanio);
	memcpy(bytes, contenido+offset, tamanio);
	return bytes;
}

tabla_de_frame_entry* obtenerEntradaDeFrame(int nroFrame){
	int i;
	for(i=0; i<list_size(tablaDeFrames->entradas);i++){
		tabla_de_frame_entry* actual = list_get(tablaDeFrames->entradas, i);
		if(actual->nroFrame == nroFrame){
			return actual;
		}
	}
	return NULL;
}

void escribirEnFrame(char* buffer, int offset, int tamanio, int frame){
	tabla_de_frame_entry* entrada = obtenerEntradaDeFrame(frame);
	memcpy(entrada->direccion_real + offset, buffer, tamanio);
}

char* obtenerBytesDeMemoriaPrincipal(int frame, int offset, int tamanio){
	int i;
	for(i=0; i<list_size(tablaDeFrames->entradas);i++){
		tabla_de_frame_entry* actual = list_get(tablaDeFrames->entradas, i);
		if(actual->nroFrame == frame){
			return obtenerBytes(actual->direccion_real, offset, tamanio);
		}
	}
	return NULL;
}

bool paginaNoPresente(void* entrada){
	tabla_de_paginas_entry* entradaDePagina = (tabla_de_paginas_entry*) entrada;
	return !entradaDePagina->presente;
}

tabla_de_paginas_entry* obtenerEntradaDePagina(int pid, int nroPagina){
	tabla_de_paginas* tablaDePaginas = buscarPorPID(tablasDePaginas, pid);
	tabla_de_paginas_entry* entrada = buscarEntradaPorPagina(tablaDePaginas, nroPagina);
	return entrada;
}

int obtenerFrameDisponible(){
	int i;
	for(i=0; i<list_size(tablaDeFrames->entradas);i++){
		tabla_de_frame_entry* actual = list_get(tablaDeFrames->entradas, i);
		if(!actual->ocupado){
			return actual->nroFrame;
		}
	};
	return -1;
}

void cargarEnMemoriaPrincipal(char* pagina, int nroFrame){
	tabla_de_frame_entry* entrada = obtenerEntradaDeFrame(nroFrame);
	memcpy(entrada->direccion_real, pagina, marco_size);
}

void cargarPagina(int nroPagina, int pid, char* pagina){
	tabla_de_paginas_entry* entrada = obtenerEntradaDePagina(pid, nroPagina);
	int nroFrameACargar = obtenerFrameDisponible();
	if(nroFrameACargar == -1){
		puts("Error: memoria llena");
		return;
	}
	cargarEnMemoriaPrincipal(pagina, nroFrameACargar);
	entrada->nroFrame = nroFrameACargar;
	entrada->presente = 1;
	return;
}

int actualizarTLB(int nroPagina, int pid){
	return 0;
}

int buscarEnTLB(int nroPagina, int pid){
	return 0;
}


void recibirAlmacenarPaginas(int cpu_socket, int pidActivo){
	int32_t nroPagina;
	int32_t offset;
	int32_t tamanio;
	char* buffer;
	char* respuesta;

	if (recv(cpu_socket, &nroPagina, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if (recv(cpu_socket, &offset, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if (recv(cpu_socket, &tamanio, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	buffer = malloc(tamanio);
	if (recv(cpu_socket, buffer, tamanio, 0) == -1) {
		perror("recv");
		exit(1);
	}

	if(TLBEnable){
		//Buscar en TLB. Si esta, ir a buscar a memoria y enviar bytes
		int frame = buscarEnTLB(nroPagina, pidActivo);
		if(frame!=-1){
			escribirEnFrame(buffer, offset, tamanio, frame);
			respuesta = string_itoa(RESPUESTA_OK);
			goto enviarBytes;
		}
	}

	// else: buscar en Tabla de Paginas del pid
	tabla_de_paginas* tablaDePaginas = buscarPorPID(tablasDePaginas, pidActivo);
	tabla_de_paginas_entry* entrada = buscarEntradaPorPagina(tablaDePaginas, nroPagina);
	if(entrada==NULL){
		respuesta = string_itoa(RESPUESTA_FAIL); //no existe tal nroPagina para tal pid
		goto enviarBytes;
	}
	if(entrada->presente){
		// esta en tabla de paginas ⇒ ir a buscar a Memoria Principal
		escribirEnFrame(buffer, offset, tamanio, entrada->nroFrame);
		// y actualizar TLB
		if(TLBEnable) actualizarTLB(nroPagina, pidActivo);
	}

	// no esta ⇒ pedir a Swap
	char* paginaSolicitada = pedirPaginaASwap(nroPagina, pidActivo);
	if(paginaSolicitada==NULL){
		respuesta = string_itoa(RESPUESTA_FAIL); //no existe tal nroPagina para tal pid
		goto enviarBytes;
	}
	// TODO cargar pagina y actualizar tabla de paginas
	// reemplazar si es necesario (Clock y clock modificado)
	cargarPagina(nroPagina, pidActivo, paginaSolicitada);
	escribirEnFrame(buffer, offset, tamanio, entrada->nroFrame);
	respuesta = string_itoa(RESPUESTA_OK);

	//Actualizar TLB
	if(TLBEnable) actualizarTLB(nroPagina, pidActivo);

	enviarBytes:
		if (send(cpu_socket, respuesta, tamanio, 0) == -1) {
				perror("send");
				exit(1);
		}
}

void recibirSolicitarPaginas(int cpu_socket, int pidActivo){
	char* bytesAEnviar;
	int32_t nroPagina;
	int32_t offset;
	int32_t tamanio;

	if (recv(cpu_socket, &nroPagina, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if (recv(cpu_socket, &offset, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if (recv(cpu_socket, &tamanio, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}

	if(TLBEnable){
		//Buscar en TLB. Si esta, ir a buscar a memoria y enviar bytes
		int frame = buscarEnTLB(nroPagina, pidActivo);
		if(frame!=-1){
			bytesAEnviar = obtenerBytesDeMemoriaPrincipal(frame, offset, tamanio);
			goto enviarBytes;
		}
	}

	// else: buscar en Tabla de Paginas del pid
	tabla_de_paginas* tablaDePaginas = buscarPorPID(tablasDePaginas, pidActivo);
	tabla_de_paginas_entry* entrada = buscarEntradaPorPagina(tablaDePaginas, nroPagina);
	if(entrada==NULL){
		bytesAEnviar = string_itoa(RESPUESTA_FAIL); //no existe tal nroPagina para tal pid
		goto enviarBytes;
	}
	if(entrada->presente){
		// esta en tabla de paginas ⇒ ir a buscar a Memoria Principal
		bytesAEnviar = obtenerBytesDeMemoriaPrincipal(entrada->nroFrame, offset, tamanio);
		// y actualizar TLB
		if(TLBEnable) actualizarTLB(nroPagina, pidActivo);
	}

	// no esta ⇒ pedir a Swap
	char* paginaSolicitada = pedirPaginaASwap(nroPagina, pidActivo);
	if(paginaSolicitada==NULL){
		bytesAEnviar = string_itoa(RESPUESTA_FAIL); //no existe tal nroPagina para tal pid
		goto enviarBytes;
	}
	// TODO cargar pagina y actualizar tabla de paginas
	// reemplazar si es necesario (Clock y clock modificado)
	cargarPagina(nroPagina, pidActivo, paginaSolicitada);
	bytesAEnviar = obtenerBytes(paginaSolicitada, offset, tamanio);

	//Actualizar TLB
	if(TLBEnable) actualizarTLB(nroPagina, pidActivo);

	enviarBytes:
		if (send(cpu_socket, bytesAEnviar, tamanio, 0) == -1) {
				perror("send");
				exit(1);
		}
}

void recibirCambioDeProcesoActivo(int cpu_socket, int* pidActivo){
	int32_t pid;
	if (recv(cpu_socket, &pid, sizeof(int32_t), 0) == -1) {
		perror("recv");
		exit(1);
	}
	*pidActivo = pid;
}

void *gestionarCPU(void* socket){
	printf("Creado hilo de gestión de CPU\n");
	printf("De socket: %d\n", (int)socket);

	int pidActivo = 0;
	int32_t headerInt;
	char* header = malloc(HEADER_SIZE);
	while(1){
		if (recv((int)socket, header, HEADER_SIZE, 0) == -1) {
			perror("recv");
			exit(1);
		}
		memcpy(&headerInt, header, sizeof(int32_t));
		if(headerInt==HEADER_ALMACENAR_PAGINAS){
			recibirAlmacenarPaginas((int)socket, pidActivo);
			continue;
		}
		if(headerInt==HEADER_SOLICITAR_PAGINAS){
			recibirSolicitarPaginas((int)socket, pidActivo);
			continue;
		}
		if(headerInt==HEADER_CAMBIO_PROCESO_ACTIVO){
			recibirCambioDeProcesoActivo((int)socket, &pidActivo);
			continue;
		}
	}

	return 0;
}

void *gestionarNucleo(void* socket){
	printf("Creado hilo de gestión de Nucleo\n");
	printf("De socket: %d\n", (int)socket);

	int32_t headerInt;
	char* header = malloc(HEADER_SIZE);
	while(1){
		if (recv((int)socket, header, HEADER_SIZE, 0) == -1) {
			perror("recv");
			exit(1);
		}
		memcpy(&headerInt, header, sizeof(int32_t));
		if(headerInt==HEADER_INIT_PROGRAMA){
			recbirInitPrograma((int)socket);
			continue;
		}
		if(headerInt==HEADER_FIN_PROGRAMA){
			recibirFinalizarPrograma((int)socket);
			continue;
		}
	}

	return 0;
}

int makeHandshake(int new_socket){
	int tipo = 0;
	//recibir Header == HEADER_HANDSHAKE
	if(tipo==-1){
		puts("Hubo error recibiendo handshake\n");
		return -1;
	}
//	if(devolver_handshake(new_socket, tipo) == -1){
//		puts("Hubo error en el handshake");
//		return -1;
//		}
//		return tipo;
//	}
	return 1;
}

int crearHiloDeComponente(int tipo, int new_socket){
	pthread_t newThread;
	int creacion;
	if(tipo==TIPO_NUCLEO){
	creacion = pthread_create(&newThread, &attr, &gestionarNucleo, (void*) new_socket);
	}
	if(tipo==TIPO_CPU){
	creacion = pthread_create(&newThread, &attr, &gestionarCPU, (void*) new_socket);
	}
	return creacion;
}

int aceptarNuevaConexion(){
	int new_socket;
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;
	char remoteIP[INET6_ADDRSTRLEN];
	// handle new connections
	addrlen = sizeof remoteaddr;
	new_socket = accept(listener,
	(struct sockaddr *)&remoteaddr,
	&addrlen);
	puts("Conexion aceptada");
	if (new_socket == -1) {
	perror("accept");
	}
	printf("umc: new connection from %s on "
	"socket %d\n",
	inet_ntop(remoteaddr.ss_family,
	get_in_addr((struct sockaddr*)&remoteaddr),
	remoteIP, INET6_ADDRSTRLEN),
	listener);
	return new_socket;
}

void manejarNuevasConexiones(){
	int new_socket = aceptarNuevaConexion();
	int tipo = makeHandshake(new_socket);
	if(tipo == -1){
		puts("Hubo error en handshake");
		return;
	}
	int creacion = crearHiloDeComponente(tipo, new_socket);
	if(creacion == -1){
		puts("Error al crear hilo\n");
	}
}

void initiMemoriaPrincipal(int cantMarcos, int marco_size){
	memoria_bloque = malloc(cantMarcos*marco_size); //char* que va a tener el contenido de todas las paginas

	tablaDeFrames = malloc(sizeof(tablaDeFrames));
	tablaDeFrames->entradas = list_create();
	int i;
	for(i=0; i<cantMarcos; i++) {
		tabla_de_frame_entry* entrada = malloc(sizeof(tabla_de_frame_entry));
		entrada->nroFrame = i;
		entrada->ocupado = 0;
		entrada->referenciado = 0;
		entrada->direccion_real = &memoria_bloque[i*marco_size];
		list_add(tablaDeFrames->entradas, entrada);
		free(entrada);
	}

	tablasDePaginas = list_create();
}

void initTLB(int cantidad_entradas_tlb){
	if(cantidad_entradas_tlb==0) return;

	TLB = malloc(sizeof(tlb));
	TLB->entradas = malloc(sizeof(tlb_entry)*cantidad_entradas_tlb);
	int i;
	for(i=0;i<cantidad_entradas_tlb;i++){
		tlb_entry* entrada = malloc(sizeof(tlb_entry));
		entrada->frame = 0;
		entrada->nroPagina = 0;
		entrada->pid = 0;
		TLB->entradas[i] = *entrada;
		free(entrada);
	}
	TLB-> usedPages = queue_create();
	*TLBEnable = 1;
}

void initSwap(char* puerto_swap){
	swap_socket = crear_socket_cliente("utnso40", puerto_swap);
	//char* cantPaginas = string_itoa(marco_size);
	//handshake(swap_socket, marco_size);
	crearHiloDeComponente(TIPO_SWAP, swap_socket);
}

int main(void) {
	t_config* config = config_create("umc.config");
	if(config==NULL){
		printf("No se pudo leer la configuración");
		return EXIT_FAILURE;
	}
	char* puerto_cpu_nucleo = config_get_string_value(config, "PUERTO_CPU_NUCLEO"); //puerto escucha de Nucleo y CPU
	char* ip_swap = config_get_string_value(config, "IP_SWAP");
	char* puerto_swap = config_get_string_value(config, "PUERTO_SWAP"); //puerto escucha de swap
	cantidad_de_marcos = config_get_int_value(config, "MARCOS");
	marco_size = config_get_int_value(config, "MARCO_SIZE");
	marcos_x_proc = config_get_int_value(config, "MARCOS_X_PROC");
	int cantidad_entradas_tlb = config_get_int_value(config, "ENTRADAS_TLB");
	// int retardo = config_get_int_value(config, "RETARDO");

	printf("Config: PUERTO_CPU_NUCLEO=%s\n", puerto_cpu_nucleo);
	printf("Config: IP_SWAP=%s\n", ip_swap);
	printf("Config: PUERTO_SWAP=%s\n", puerto_swap);

	initiMemoriaPrincipal(cantidad_de_marcos, marco_size);
	initTLB(cantidad_entradas_tlb);

	initSwap(puerto_swap);

	listener = crear_puerto_escucha(puerto_cpu_nucleo);
	printf("Creado listener: %d\n", listener);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//TODO
	// Crear hilo para consola

	while(1){
		puts("Esperando conexiones...");
		manejarNuevasConexiones(); //Nucleo o CPU
	}

	close(listener); // bye!
	puts("Terminé felizmente");
	return EXIT_SUCCESS;
}
