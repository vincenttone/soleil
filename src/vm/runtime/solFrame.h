#ifdef _SOL_FRAME_H_
#define _SOL_FRAME_H_ 1

typedef struct _SolFrame_ {
	int flag;
	int _pc;
	SolFrame *frames;
	SolFrame *previous;
	SolLocals locals;
	SolStackGroup *groups;
} SolFrame



#endif
