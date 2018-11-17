extern int interruptCount;

// This structure is used to access registers stored on stack by
// "interrupt" modifier.
struct registers{
	int bp;
	int di;
	int si;
	int ds;
	int es;
	int dx;
	int cx;
	int bx;
	int ax;
	int ip;
	int cs;
	int flags;
	int retip;
	int retcs;
};

#define MAX_THREADS 100

// Possible thread states
#define READY 1
#define RUNNING 2
#define EMPTY 3
#define SUSPEND 4
#define SEMBLOCK 5

#define bool int
#define true 1
#define false 0

#define SYSCALL int
#define SYSOK  1
#define SYSERR  (-1)
#define CS_START -1
#define CS_STOP -2

// Interrupt enable/disable functions
void cli();
void sti();
void interrupt contextSwitcher(...);

/*********************************************************
									THREAD TABLE FUNCTIONS
*********************************************************/
#define THREAD void far

// Structure to hold different thread-related information
// including context, priority, signal disposition etc.

struct threadTableType{
	int priority;     //thread priority
	struct registers context;    //context
	int ss, sp;  //stack segment and offset
	char far *stack;  //pointer to stack buffer
	int state;
	int prev,next;
	char name[20];
	int sigCount; //keep track of number of pending signals
	int signals[16]; //signal no. pending for the thread
};

extern int topThreadEntry, freeThreadEntry;
//extern threadTableType threadTable[MAX_THREADS];
extern threadTableType threadTable[MAX_THREADS];

void initThreadTable();
int getFreeThreadEntry();
void releaseThreadEntry(int id);

/*********************************************************
									THREAD FUNCTIONS
*********************************************************/
extern int currentTID; // Id of the currently executing thread

// Thread creation and manipulation functions
int createThread(THREAD(*thread)(),int priority, int stacksize, char *name);
SYSCALL resumeThread(int id);
SYSCALL suspendThread(int id);
SYSCALL readyThread(int id);

// Thread clean up functions
void releaseThreadResources();
THREAD nullThread();

void initSOS();
