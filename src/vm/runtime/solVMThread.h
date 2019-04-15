#ifdef _SOL_VM_THREAD_H_
#define _SOL_VM_THREAD_H_ 1

#include "solVMFrame"

typedef struct {
	int pc;
	SolMainHeap *heap;
	SolVMFrame *frames;
} SolVMThread

#endif
