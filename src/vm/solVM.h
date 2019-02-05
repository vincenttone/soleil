#ifdef _SOL_VM_H_
#define _SOL_VM_H_ 1

typedef struct _SolVM_ {
	int pc;
	SolMemManager *heap;
	SolMethod *methodArea;
} SolVM

#endif
