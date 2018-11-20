#include "kernel.h"
#include <string.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

int interruptCount;
int currentTID;
int semIndex;
int signo;

int signalCall = false;
int rrr = true;
int synch = false;

void interrupt (*oldInt8)(...);

DEFINE_PRIO_QUEUE(ready,MAX_THREADS);

int id1, id2;

void cli() {
	// interruptCount++;
	asm {cli}
}

void sti() {
	// interruptCount--;
	asm {sti}
}

void interrupt contextSwitcher(...) {

	oldInt8();

	if (currentTID == CS_STOP)
		return;

	cli();

	if (signalCall) {

		// _SP value could go out because of local variables. Use _BP instead
		registers *r = (registers*)(_BP);
		// Get the signal entry from the signal mapping table
		signo = threadTable[currentTID].signals[threadTable[currentTID].sigCount-1];

		// Forcing to execute handler at mapping location 'signo'
		// IMPORTANT : +16 is used to access the handler code without
		// forcing the register stacking
		r->ip = FP_OFF(signalMap[signo]) + 16;
		r->cs = FP_SEG(signalMap[signo]);
		threadTable[currentTID].sigCount--;
		signalCall=false;
	} else {
		// cli();
		if (currentTID != CS_START) {
			threadTable[currentTID].ss = _SS;
			threadTable[currentTID].sp = _SP;

			if (threadTable[currentTID].state == RUNNING) {
				addPrioQueue(ready,currentTID,threadTable[currentTID].priority);
				threadTable[currentTID].state = READY;
			}
		}

		int id = removePrioQueue(ready);
		threadTable[id].state = RUNNING;
		_SS = threadTable[id].ss;
		_SP = threadTable[id].sp;
		currentTID = id;
	}

	//Check if there is any pending signal
	if (threadTable[currentTID].sigCount) {
		signalCall = true;
		contextSwitcher();
	}

	sti();
}

void interrupt signalProc() {
	char far *screen = (char far*)MK_FP(0xb800,0);
	screen[2] = 'X';
	screen[3] = 31;
}

SIGDEF testProc() {
	char far *screen = (char far*)MK_FP(0xb800,0);
	screen[4] = 'Y';
	screen[5] = 31;
}

// Define system idle process
// This process will be an infinite loop process
// displaying a counter in upper-left corner of screen

THREAD nullThread() {
	char far* screen = (char far*)MK_FP(0xB800,0);
	int counter = 0;

	while(1) {
		screen[0] = counter + '0';
		screen[1] = 31;
		counter = (counter + 1) % 10;

		/*
		int b = 0, c = 0;
		if (synch) {
				semWait(semIndex);
				screen[4]='P'; screen[5]=31;
				semRelease(semIndex);
		}
		c++;
		if ((c%2000)==0){
			if (b)
				resumeThread(id2);
			else
				suspendThread(id2);
			c=0;b=!b;
		}
		*/
	}
}

THREAD nullThread2(){
	char far *screen = (char far*)MK_FP(0xB800,160);
	int counter = 0;

	while(1){
		screen[0] = counter+'0';
		screen[1] = 31;
		counter = (counter + 1) % 10;

/*		if (b==2000 && rrr){
			semWait(semIndex);
			screen[4]='S';screen[5]=31;
			synch=true;
			sleep(10);
			synch=false;
			semRelease(semIndex);
			rrr=false;
		}*/
	}
}

void initSOS() {

	// interruptCount = 0;

	initSignal();
	initSem();
	initMem(MEM_SIZE);
	initThreadTable();
	initPrioQueue(ready,MAX_THREADS);

//	setSignal(0,signalProc);
	semIndex = semCreate(1);
	resumeThread(id1 = createThread(nullThread,400,1024,"NullThread"));
	resumeThread(id2 = createThread(nullThread2,400,1024,"NullThread"));

	currentTID = CS_START;
	oldInt8 = getvect(8);
	setvect(8,contextSwitcher);
}

main() {
	//clrscr();
	initSOS();
	while(1);
}
