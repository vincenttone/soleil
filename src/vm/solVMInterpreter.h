#ifndef _SOL_VM_INTERPRETER_H_
#define _SOL_VM_INTERPRETER_H_ 1

#include "solRegister.h"
#include "solByteCode.h"

typedef struct {
	int ip;
	solByte* code;
	size_t codeSize;
	SolVMFrame* frame;
	SolVMFunc* enter;
} SolVMInterpreter;

int solVMInterpreter_process(SolVMInterpreter*);

#define solVMInterpreter_trace() do{}while(0)

#endif
