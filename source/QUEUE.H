struct queueType{
  int prev, key, next;
};

#define DEFINE_PRIO_QUEUE(var, size)  queueType var[size+1]

void initPrioQueue(queueType q[],int size);
void addPrioQueue(queueType q[],int index,int key);
int removePrioQueue(queueType q[]);
SYSCALL removePrioQueueByID(queueType q[],int index);
bool isEmptyPrioQueue(queueType q[]);
