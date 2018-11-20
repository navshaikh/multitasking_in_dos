#include "kernel.h"
#include <string.h>
#include <dos.h>

extern queueType ready[];

// Create a new entry in ThreadTable, in suspended state
// and returns the ThreadID

int createThread(THREAD(*thread)(), int priority, int stacksize, char *name) {
	int id = getFreeThreadEntry();

	if (id == SYSERR)
		return SYSERR;

	cli();
	threadTable[id].priority = priority;
	threadTable[id].state = SUSPEND;
	strcpy(threadTable[id].name,name);
	//threadTable[id].stack = new char[stacksize];
	threadTable[id].stack = getmem(stacksize);

	if (!threadTable[id].stack) {
		releaseThreadEntry(id);
		sti();
		return SYSERR;
	}

	threadTable[id].ss = FP_SEG(threadTable[id].stack);

	registers *r = (registers*)(threadTable[id].stack+stacksize-sizeof(registers));

	r->ax=0;r->bx=0;r->cx=0;r->dx=0;
	r->es=_ES; r->ds=_DS;r->si=_SI;r->di=_DI;
	r->bp=_BP;r->ip=FP_OFF(thread);r->cs=FP_SEG(thread);
	r->flags=0xFFFF;
	r->retip=FP_OFF(releaseThreadResources);
	r->retcs=FP_SEG(releaseThreadResources);

	threadTable[id].sp = FP_OFF(threadTable[id].stack+stacksize-sizeof(registers));
	threadTable[id].sigCount = 0;
	sti();
	return id;
}

void releaseThreadResources() {
	cli();
//	delete threadTable[currentTID].stack;
	freemem((char *)threadTable[currentTID].stack,1024);
	releaseThreadEntry(currentTID);

	currentTID = CS_START;

	char far*s = (char far*)MK_FP(0xB800,240);
	s[0]='A';
	s[1]=31;
	sti();
}

SYSCALL resumeThread(int id) {
	if (threadTable[id].state != SUSPEND)
		return SYSERR;

	cli();
	threadTable[id].state = READY;
	addPrioQueue(ready,id,threadTable[id].priority);
	sti();
	return SYSOK;
}

SYSCALL suspendThread(int id) {
	if (threadTable[id].state == READY)
		return SYSERR;

	cli();
	threadTable[id].state = SUSPEND;
	removePrioQueueByID(ready,id);
	sti();
	return SYSOK;
}

SYSCALL readyThread(int id) {
	cli();
	threadTable[id].state = READY;
	addPrioQueue(ready,id,threadTable[id].priority);
	sti();
	return SYSOK;
}
