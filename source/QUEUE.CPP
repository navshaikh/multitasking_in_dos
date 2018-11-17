#include "kernel.h"
#include "queue.h"

void initPriorityQueue(queueType q[],int size) {
	q[0].next = -1;
	q[0].key = size;
}

void addPriorityQueue(queueType q[],int index, int key) {
	cli();
	index++;
	q[index].key = key;

	if(q[0].next == -1) {
		q[0].next = index;
		q[index].next = q[index].prev = -1;
	} else {
		int track = q[0].next;
		while(q[track].key<=key && q[track].next!=-1)
			track = q[track].next;

		if (q[track].key <= key) {
			q[index].prev = track;
			q[index].next = q[track].next;

			if (q[track].next != -1)
				q[q[track].next].prev = index;

			q[track].next = index;
		}
		else {
			q[index].next = track;
			q[index].prev = q[track].prev;

			if (q[track].prev != -1)
				q[q[track].prev].next = index;

			q[track].prev = index;
		}
	}

	if (q[q[0].next].key > key)
		q[0].next = index;

	sti();
}

int removePriorityQueue(queueType q[]) {
	if (q[0].next == -1)
		return SYSERR;

	cli();
	int index = q[0].next;
	q[0].next = q[index].next;

	if (q[0].next != -1)
		q[q[0].next].prev = -1;

	index--;
	sti();
	return index;
}

SYSCALL removePriorityQueueByID(queueType q[], int index) {
	if(q[0].next == -1)
		return SYSERR;

	cli();
	index++;
	if (q[0].next == index) q[0].next=q[index].next;
	if (q[index].next != -1) q[q[index].next].prev = q[index].prev;
	if (q[index].prev != -1) q[q[index].prev].next = q[index].next;
	sti();
	return SYSOK;
}

bool isEmptyPriorityQueue(queueType q[]) {
	return q[0].next == -1 ? true : false;
}
