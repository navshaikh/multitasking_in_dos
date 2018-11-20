#include "kernel.h"
#include "mem.h"
#include <alloc.h>
#include <process.h>

#define MEM_SIZE 1024

#define WORD unsigned int

#define roundew(x) ((3 + (WORD)(x)) & (~3))
#define truncew(x) (((WORD)(x)) & (~3))

struct memType *memList;
char *memAlloc;

void initMem(int nBytes) {
	int memSize = roundew(MEM_SIZE);
	struct memType *memPtr;

	if (!(memAlloc = (char *)malloc(memSize)))
		exit(1);

	memList->next = memPtr = (struct memType*)memAlloc;
	memPtr->memLen = nBytes - sizeof (struct memType);
	memPtr->next = NULL;
}

char *getmem(int nBytes) {
	struct memType *p,*q,*leftover;

	nBytes = roundew(nBytes);

	for(q=memList, p=q->next; (char*)p!=NULL; q=p,p=p->next) {
		if(p->memLen == nBytes) {
			q->next = p->next;
			return ((char*)p);
		}
		else if (p->memLen> nBytes) {
			leftover = (struct memType *)((char*)p+nBytes);
			q->next = leftover;
			leftover->next = p->next;
			leftover->memLen = p->memLen - nBytes;
			return ((char*)p);
		}
	}

	return (NULL);
}

SYSCALL freemem (char* block, int size) {
	struct memType *p, *q;
	char *top;

	if (block == NULL || size < 0 || size > MEM_SIZE)
		return SYSERR;

	(char*)q = NULL;

	for (p=memList->next; (char*)p!=NULL && (char*)p < block; q=p, p=p->next);

	if ((char*)q!=NULL && (top=(char*)q+q->memLen)>block
		|| (char*)p!=NULL && (block+size) > (char*)p) {
		  return SYSERR;
	}

	if ((char*)q!=NULL && top == block)
		q->memLen += size;

	else {
		// Make this block a head
		if (q == NULL){
			((struct memType*)block)->memLen = size;
			((struct memType*)block)->next = p;
			memList->next = (struct memType*)block;
			(char*)q = block;
		}

		else if ((char*)q != NULL && top < block) {
			((struct memType*)block)->memLen = size;
			((struct memType*)block)->next = q->next;
			q->next = (struct memType*)block;
			(char*)q = block;
		}
	}

	// Merge the blocks
	if ((char*)p!=NULL && ((char*)q + q->memLen) == (char*)p) {
		q->memLen += p->memLen;
		q->next = p->next;
	}
	return SYSOK;
}

int getFreeSize() {
	struct memType *p, *q;
	int total = 0;
	for (p=memList->next;(char*)p!=NULL; p=p->next) {
		total += p->memLen;
	}
	return (total);
}
