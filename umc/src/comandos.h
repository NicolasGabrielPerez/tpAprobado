#ifndef COMANDOS_H_
#define COMANDOS_H_

#include "umc-structs.h"

	void setRetardo(int retardo);

	void dumpAllTables();

	void dumpTable(int pid);

	void dumpAllData();

	void dumpData(int pid);

	void flushTlb();

	void flushMemory(int pid);

#endif /* COMANDOS_UMC_H_ */
