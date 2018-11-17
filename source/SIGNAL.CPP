#include "kernel.h"
#include "signal.h"
#include <dos.h>

//At present only max of signals could be defined
#define MAX_SIG 16

#define isbadSignal(x) (x<0||x>MAX_SIG)

// Inititialize signal table. Set the default mapping of signals to terminate
char far* signalMap[16];

void initSignal() {
	for(int i=0; i<MAX_SIG; i++)
		signalMap[i] = (char far*)releaseThreadResources;
}

SYSCALL setSignal(int signo, SIGDEF(*newhandler)()) {

	if (isbadSignal(signo))
		return SYSERR;

	signalMap[signo] = (char far*)newhandler;
	return SYSOK;
}

SYSCALL setDefaultSignal(int signo) {
	if (isbadSignal(signo))
		return SYSERR;

	signalMap[signo] = (char far*)releaseThreadResources;
	return SYSOK;
}

SYSCALL sendSignal(int thread_id, int signo) {
	cli();

	if (isbadSignal(signo))
		return SYSERR;

	threadTable[thread_id].sigCount++;
	threadTable[thread_id].signals[threadTable[thread_id].sigCount-1] = signo;
	sti();
	return SYSOK;
}
