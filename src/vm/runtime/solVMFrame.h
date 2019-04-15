#ifdef _SOL_VM_FRAME_H_
#define _SOL_VM_FRAME_H_ 1

typedef struct _SolFrame_ {
	int flag;
	int _pc;
	SolLocals *_heap;
	SolVMStack *_stack;
	SolVM *vm;
	SolFrame *pre;
} SolFrame

int SolVMFrameNew(SolVM*);
int SolVMFrameFree();

int SolVMFrameLoad(SolVMFrame*); // load frame to registers
int SolVMFrameRun(SolVMFrame*);

#endif
