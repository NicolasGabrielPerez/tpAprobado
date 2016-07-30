#ifndef NOTIFY_H_
#define NOTIFY_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/log.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sockets/sockets.h>
#include <sockets/config.h>
#include <sockets/pcb.h>
#include <pthread.h>
#include "nucleo-structs.h"


void* checkEvent();

#endif /* IO_DEVICE_H_ */
