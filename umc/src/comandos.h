#ifndef COMANDOS_H_
#define COMANDOS_H_

#include "umc-structs.h"

	void setRetardo(int retardo);

	void dumpAllTables();

	void dumpTable(int pid);

	void dumpFrames();

	void dumpAllData();

	void dumpData(int pid);

	void showActive();

	void flushTlb();

	void dumpTlb();

	void flushMemory(int pid);

#endif /* COMANDOS_UMC_H_ */
