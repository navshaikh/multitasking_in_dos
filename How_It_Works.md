## How it works?
### Multitasking
A task, which is to be run as a preemptive multitasking 'thread' alongside other tasks, is defined and declared as a C function in a program. This function runs in the context of thread and register definitions shown below. Terminate and Stay Resident (TSR) is used to override the Interrupt Service Routine (ISR) for *Timer* interrupt pointing to a new definition of *Context Switcher*. This allows the system to control the tasks and manage their executions when *Timer* interrupt is generated.

``` C++
struct registers {
	int bp; int di;
	int si; int ds;
	int es; int dx;
	int cx; int bx;
	int ax; int ip;
	int cs; int flags;
	int retip; int retcs;
};

struct threadTableType {
	int priority;             // Thread priority
	struct registers context; // Context
	int ss, sp;               // Stack segment and offset
	char far *stack;          // Pointer to stack buffer
	int state;
	int prev, next;           // Linked list
	char name[20];            // Thread name
	int signal;               // Pending signal
};
```
A main program can define multiple tasks, each as a C function, and request to create them using `createThread`.

``` C++
// Task definition
THREAD printingTask() {
  ...
}

// Register and initialize task indicating priority (1) and memory allocation (1024 bytes)
createThread(printingTask, 1, 1024, "Printing Thread")
```

When a task is initialized, following happens:
- Requested memory is allocated (if available)
- Task is registered in the thread table
- Interrupt Pointer (IP) and Code Segment (CS) of registers associated with this task point to the C function (e.g: printingTask)
- Task is queued for execution

When *Timer* interrupt is generated, the context of currently executing task is saved and queued. Next task is dequeued taking into account task priority and its execution is resumed when ISR returns.

This way multitasking is achieved in DOS environment.

### Memory Management - A FIFO-based Approach

FIFO is the simple and popular technique for implementing memory management. The idea is to override C-defined `malloc` and `delete` for fine-grained control, substituting them with new functions for allocating and de-allocating memory. When the memory manager is initialized, it allocates a heap and uses it to allocate memory for each requesting task. The manager keeps track of allocation and de-allocation through two linked lists – a ‘free linked list’ (FLL) sorted on the block size holding pointers to memory area(s) for the *free* blocks and an ‘allocated linked list’ (ALL) holding access to memory blocks for blocks allocated to different tasks in execution.

Each node of these two linked lists has a pointer to the memory area and allocated size. When a memory block is requested, the function looks up the appropriate size block from FLL, updates this list, adds the entry in ALL and returns the pointer to the memory segment (equal to the request size). Similarly, when the memory is reclaimed using the deallocation function, the memory block is returned back to FLL and is deleted from ALL. The manager also checks if this new entry in FLL can be merged with its neighbors (akin to defragmentation).

### Signal Implementation
Signals inform processes of the occurrence of asynchronous events. If a signal is sent to a task, the kernel sets a bit in the signal field of the thread table corresponding to the type of signal received. The kernel checks for the receipt of a signal when a task is about to return from SLEEP to RUNNING state. If a pending signal is detected, the specified signal handler should be executed prior to resuming the task.

When a signal is sent and a task is scheduled to be resumed, the interrupt function (contextSwitcher(…)) detects a pending signal and recursively calls itself. It overwrites Instruction Pointer (IP) and Code Segment (CS) values with the address of signal handler function to force the execution of the signal handler before resuming the task. The return address of this handler (the signal function), however, points either to the owner task address or to another other signal handler in case another signal is sent to this task.

If a signal doesn’t specify any handler, the default signal disposition is to terminate the task.

The synopsis of implemented system calls for the signals are :
```C++
SYSCALL setSignal(int signo, void (*newhandler()))
SYSCALL setDefaultSignal(int signo)
SYSCALL sendSignal(int thread_id, it signo)
```
### Synchronization Constructs
#### Semaphores
Semaphores are implemented using static arrays and provide the mechanism by which system can guarantee that a task in critical section is safe from the intervention of other tasks, which can possibly corrupt/update its data. Semaphore implementation provides system calls to support  *create*, *wait*, *release* and *destroy* functions on our semaphores. These are:
```C++
int semCreate(int initValue)
SYSCALL semWait(int handle)
SYSCALL semRelease(int handle)
SYSCALL semDestroy(int handle)
```

#### Messages
Messages are used  for interprocess communication (IPC) and for cooperative multitasking. The message construct in this system are synchronous, which means it blocks the caller if the message buffer is not empty unless a forced message is sent. Following functions are offered to support messaging:
```C++
SYSCALL sendMessage(int thread_id, int message)
SYSCALL recvMessage(int *msg)
SYSCALL sendForceMessage(int thread_id, int message)
```
