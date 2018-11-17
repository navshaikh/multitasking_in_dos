#include "kernel.h"
#include "semaphor.h"

#define isBadHandle(x) (x < 0 || x > MAX_SEM)

struct semType semaphore[MAX_SEM];

void initSem() {
	for (int i=0; i<MAX_SEM; i++) {
		semaphore[i].status = EMPTY;
		semaphore[i].count = 0;
	}
}

int semCreate(int initial_value) {
	int index = -1;
	while(semaphore[++index].status!=EMPTY);

	semaphore[index].status = !EMPTY;
	semaphore[index].count = initial_value;
	return (index);
}

SYSCALL semWait(int handle) {
	if (isBadHandle(handle))
		return SYSERR;

	cli();

	if (semaphore[handle].count <= 0) {
		addPrioQueue(semaphore[handle].blocking, currentTID,
									threadTable[currentTID].priority);
		threadTable[currentTID].state = SEMBLOCK;
		semaphore[handle].count--;
		sti();
		contextSwitcher();
	}

	semaphore[handle].count--;
	sti();
	return SYSOK;
}

SYSCALL semRelease(int handle) {
	if (isBadHandle(handle))
		return SYSERR;

	cli();

	if (semaphore[handle].count <= 0) {
		int id = removePrioQueue(semaphore[handle].blocking);
		threadTable[id].state = READY;
		readyThread(id);
		semaphore[handle].count++;
		sti();
		contextSwitcher();
	}

	semaphore[handle].count++;
	sti();
	return SYSOK;
}

SYSCALL semDestroy(int handle) {
	if (isBadHandle(handle))
		return SYSERR;

	semaphore[handle].count = 0;
	semaphore[handle].status = EMPTY;
	return SYSOK;
}
