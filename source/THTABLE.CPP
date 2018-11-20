#include "kernel.h"

threadTableType threadTable[MAX_THREADS];
int topThreadEntry, freeThreadEntry;

// Thread table management routines
void initThreadTable() {
	topThreadEntry = -1;
	freeThreadEntry = 0;

	for (int i=0;i<MAX_THREADS;i++) {
		threadTable[i].prev = i-1;
		threadTable[i].next = i+1;
		threadTable[i].state = EMPTY;
	}

	threadTable[MAX_THREADS-1].next = -1;
}

// This function finds a freenode in Thread table, adds it to used
// entries list and returns the index of that entry (ThreadID)
// Returns SYSERR on error

int getFreeThreadEntry() {

	if (freeThreadEntry == -1)
		return SYSERR;

	cli();
	int index = freeThreadEntry;
	freeThreadEntry = threadTable[freeThreadEntry].next;
	threadTable[freeThreadEntry].prev = -1;

	threadTable[topThreadEntry].prev = index;
	threadTable[index].next = topThreadEntry;
	topThreadEntry = index;

	sti();
	return index;
}

// Adds the specified entry to free nodes list
void releaseThreadEntry(int id) {
	cli();
	if (threadTable[id].prev != -1)
		threadTable[threadTable[id].prev].next = threadTable[id].next;

	if (threadTable[id].next != -1)
		threadTable[threadTable[id].next].prev = threadTable[id].prev;

	threadTable[id].prev = -1;
	threadTable[id].next = freeThreadEntry;
	threadTable[id].state = EMPTY;
	freeThreadEntry = id;
	sti();
}
