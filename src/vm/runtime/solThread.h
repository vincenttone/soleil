#ifdef _SOL_THREAD_H_
#define _SOL_THREAD_H_ 1

typedef struct _SolThread_ {
	int pc;
	SolMainHeap *heap;
	SolFrame *frames;
	
} SolThread

#endif
