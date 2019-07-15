#include <assert.h>
#include "solVMInterpreter.h"

int solVMInterpreter_process(SolVMInterpreter *i)
{
	int o1 = 0, o2 = 0, o3 = 0;
	SolRegister *r;
	int code = i->code[ip];
	while (opcode != _halt && i->ip < codeSize) {
		if (i->trace) {
			solVMInterpreter_trace();
		}
		switch (opcode) {
		case _iadd:
			o1 = solVMInterpreter_getOperand();
			o2 = solVMInterpreter_getOperand();
			o3 = solVMInterpreter_getOperand();
			r = solVMInterpreter_getRegister();
			*(r + o3) = *(r + o1) + *(r + o2);
			break;
		case _isub:
		default:
			assert("no such opcode");
		}
	}
}

int solVMInterpreter_getOperand(SolVMInterpreter *i)
{
    int b1 = i->code[i->ip++] & 0xFF;
	int b2 = i->code[i->ip++] & 0xFF;
	int b3 = i->code[i->ip++] & 0xFF;
	int b4 = i->code[i->ip++] & 0xFF;
	return b1<<(8*3) | b2<<(8*2) | b3<<(8*1) | b4;
}
