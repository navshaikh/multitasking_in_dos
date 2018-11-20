#define MEM_SIZE  4096

// sizeof(memList)=4 bytes
struct memType {
	struct memType *next; // 2 bytes
	int memLen; // 2 bytes
};

extern struct memType *memList;
void initMem(int nBytes);
char *getmem(int nBytes);
SYSCALL freemem(char *block,int size);
int getFreeSize();
