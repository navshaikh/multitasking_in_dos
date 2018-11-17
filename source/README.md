# Multi-tasking in DOS using Turbo C++
This source code is circa late 90s.

DOS is a non-preemptive single-tasked operating system. This project offers primitives to enable multitasking in DOS. This idea works using the concept of [Terminate and Stay Resident](https://en.wikipedia.org/wiki/Terminate_and_stay_resident_program) in DOS.

Every program or function, which is to be run as a preemptive multitasking process, is run in the context of thread and register defined in this project. TSR is used to override the Interrupt Vector Routine (ISR) for Timer with our definition of Context Switcher. This led us to customize the way we can control the threads and manage their execution on timer interrupt.

```c++
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
    int priority;             //thread priority
    struct registers context; //context
    int ss, sp;               //stack segment and offset
    char far *stack;          //pointer to stack buffer
    int state;
    int prev, next;           //linked list elements
    char name[20];            //thread name
    int signal;               //pending signal
};
```

When a program begins, it can create different threads - each executing a task (and passed into the queue), and triggers TSR and waits for the timer interrupt). As soon as the timer interrupt is generated, a thread is removed from the queue. Stack register values are loaded with that of function definition and return IP and return CS were made to point to the function address. As a result, when the call returns from the ISR, our code is executed. This way, when the timer interrupt is generated again, the context of currently executing process code is saved and the thread is passed into the queue. Another process is removed from the queue and its execution is resumed. The fragment of this part of the code along with the way this part takes care of signal is shown at the end. This way multitasking is achieved in DOS environment.
