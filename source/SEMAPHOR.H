#define MAX_SEM 10

struct semType {
	int count;
	int status;
	DEFINE_PRIO_QUEUE(blocking,MAX_THREADS);
};

extern struct semType semaphore[MAX_SEM];
void initSem();
int semCreate(int initial_value);
SYSCALL semWait(int handle);
SYSCALL semRelease(int handle);
SYSCALL semDestroy(int handle);
