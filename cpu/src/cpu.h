#ifndef CPU_H
#define CPU_H

#include <commons/log.h>

extern t_log* logger;
extern u_int32_t QUANTUM;
extern u_int32_t SLEEP_QUANTUM;

extern bool hasToReturn;
extern bool hasToFinishProgram;


void exitProgram();

#endif
