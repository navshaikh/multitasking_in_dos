/* All signal handlers need to be defined using this macro */

#define SIGDEF void interrupt

extern char far* signalMap[16];
void initSignal();
SYSCALL setSignal(int signo, SIGDEF (*newhandler)());
SYSCALL setDefaultSignal(int signo);
SYSCALL sendSignal(int thread_id,int signo);
