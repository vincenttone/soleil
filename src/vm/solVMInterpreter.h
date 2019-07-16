#ifndef _SOL_VM_INTERPRETER_H_
#define _SOL_VM_INTERPRETER_H_ 1

#include "solRegister.h"
#include "solByteCode.h"
#include "solVMBytecodeAssembler.h"

#define void* SolAddress
#define short SolShort
#define int SolInt
#define uint SolUInt
#define long SolLong
#define double SolDouble

typedef union {
	SolLong l;
	SolDouble d;
} SolVMVarl64;

typedef struct {
	int ip;
	SolVMFrame* frame;
	SolVMBytecodeStorage* storage;
} SolVMInterpreter;

int solVMInterpreter_process(SolVMInterpreter*);

#define solVMInterpreter_trace() while(0)do{}

#define solVMInterpreter_GET_ADDR(local, offset)   (*((SolAddress*)(local + offset)))
#define solVMInterpreter_GET_INT(local, offset)    (*((SolInt*)(local + offset)))
#define solVMInterpreter_GET_FLOAT(local, offset)  (*((SolFloat*)(local + offset)))
#define solVMInterpreter_GET_LONG(local, offset)   (((SolVMVal64*)(local + offset))->l)
#define solVMInterpreter_GET_DOUBLE(local, offset) (((SolVMVal64*)(local + offset))->d)

#define solVMInterpreter_SET_ADDR(local, offset, val)   ((*((SolAddress*)(local + offset))) = val)
#define solVMInterpreter_SET_INT(local, offset, val)    ((*((SolInt*)(local + offset))) = val)
#define solVMInterpreter_SET_FLOAT(local, offset, val)  ((*((SolFloat*)(local + offset))) = val)
#define solVMInterpreter_SET_LONG(local, offset, val)   ((*(((SolVMVal64*)(local + offset))->l)) = val)
#define solVMInterpreter_SET_DOUBLE(local, offset, val) ((*(((SolVMVal64*)(local + offset))->d)) = val)

#endif
