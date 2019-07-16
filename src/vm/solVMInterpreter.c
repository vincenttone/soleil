#include <assert.h>
#include "solVMInterpreter.h"

int solVMInterpreter_process(SolVMInterpreter *i)
{
	int o1 = 0, o2 = 0, o3 = 0;
	SolRegister r[] = i->frame->registers;
	int code = i->storage->code[ip];
	while (opcode != _halt && i->ip < i->storage->codeSize) {
		if (i->trace) {
			solVMInterpreter_trace();
		}
		switch (opcode) {
		case SOL_BC_NOP:
			SOL_UP_PC_N_CONTINUE(1);
		case SOL_BC_IADD:
			o1 = solVMInterpreter_getOperand();
			o2 = solVMInterpreter_getOperand();
			o3 = solVMInterpreter_getOperand();
			r[o3] = r[o1] + r[o2];
			break;
		case SOL_BC_ISUB:
			o1 = solVMInterpreter_getOperand();
			o2 = solVMInterpreter_getOperand();
			o3 = solVMInterpreter_getOperand();
			r[o3] = r[o1] - r[o2];
		case SOL_BC_IMUL:
			o1 = solVMInterpreter_getOperand();
			o2 = solVMInterpreter_getOperand();
			o3 = solVMInterpreter_getOperand();
			r[o3] = r[o1] * r[o2];
		case SOL_BC_IDIV:
			o1 = solVMInterpreter_getOperand();
			o2 = solVMInterpreter_getOperand();
			o3 = solVMInterpreter_getOperand();
			*(r + o3) = *(r + o1) / *(r + o2);
		case SOL_BC_ILT:
			o1 = solVMInterpreter_getOperand();
			o2 = solVMInterpreter_getOperand();
			o3 = solVMInterpreter_getOperand();
			r = solVMInterpreter_getRegister();
			*(r + o3) = *(r + o1) < *(r + o2);
		case SOL_BC_ILTE:
			o1 = solVMInterpreter_getOperand();
			o2 = solVMInterpreter_getOperand();
			o3 = solVMInterpreter_getOperand();
			r = solVMInterpreter_getRegister();
			*(r + o3) = *(r + o1) <= *(r + o2);
		case SOL_BC_IGT:
			o1 = solVMInterpreter_getOperand();
			o2 = solVMInterpreter_getOperand();
			o3 = solVMInterpreter_getOperand();
			r = solVMInterpreter_getRegister();
			*(r + o3) = *(r + o1) > *(r + o2);
		case SOL_BC_IGTE:
			o1 = solVMInterpreter_getOperand();
			o2 = solVMInterpreter_getOperand();
			o3 = solVMInterpreter_getOperand();
			r = solVMInterpreter_getRegister();
			*(SolInt*)(r + o3) = *(SolInt*)(r + o1) >= *(SolInt*)(r + o2);
		case SOL_BC_IEQ:
			o1 = solVMInterpreter_getOperand();
			o2 = solVMInterpreter_getOperand();
			o3 = solVMInterpreter_getOperand();
			r = solVMInterpreter_getRegister();
			*(r + o3) = *(r + o1) == *(r + o2);
		case SOL_BC_I2F:
			o1 = solVMInterpreter_getOperand();
			o2 = solVMInterpreter_getOperand();
		default:
			assert("no such opcode");
		}
	}
}

static inline solVMInterpreter_get_operand_3()
{
	
}

int solVMInterpreter_getOperand(SolVMInterpreter *i)
{
    int b1 = i->code[i->ip++] & 0xFF;
	int b2 = i->code[i->ip++] & 0xFF;
	int b3 = i->code[i->ip++] & 0xFF;
	int b4 = i->code[i->ip++] & 0xFF;
	return b1<<(8*3) | b2<<(8*2) | b3<<(8*1) | b4;
}
