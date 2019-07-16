#ifndef _SOL_BYTECODES_
#define _SOL_BYTECODES_ 1

enum SolByteCode {
	SOL_BC_ILLEGAL = -1,
	
	SOL_BC_NOP     = 0,

	// COMMAND FROM TO
	// load immediate number into register
	// const imm $reg
	SOL_BC_ICONST,
	SOL_BC_LCONST,
	SOL_BC_FCONST,
	SOL_BC_DCONST,
	// load from local variable into register
	// load addr $reg
	SOL_BC_ILOAD,
	SOL_BC_LLOAD,
	SOL_BC_FLOAD,
	SOL_BC_DLOAD,
	// store from register into local variable
	// store $reg addr
	SOL_BC_ISTORE,
	SOL_BC_LSTORE,
	SOL_BC_FSTORE,
	SOL_BC_DSTORE,

	// duplication
	// dup reg1 reg2
	SOL_BC_DUP,
	SOL_BC_SWAP,
	SOL_BC_PUSH,
	SOL_BC_POP,

	// type convertion
	SOL_BC_D2F,
	SOL_BC_D2I,
	SOL_BC_D2L,

	SOL_BC_F2D,
	SOL_BC_F2I,
	SOL_BC_F2L,

	SOL_BC_I2D,
	SOL_BC_I2F,
	SOL_BC_I2L,

	SOL_BC_L2D,
	SOL_BC_L2F,
	SOL_BC_L2I,

	// computation
	SOL_BC_IADD,
	SOL_BC_LADD,
	SOL_BC_FADD,
	SOL_BC_DADD,

	SOL_BC_ISUB,
	SOL_BC_LSUB,
	SOL_BC_FSUB,
	SOL_BC_DSUB,

	SOL_BC_IMUL,
	SOL_BC_LMUL,
	SOL_BC_FMUL,
	SOL_BC_DMUL,

	SOL_BC_IDIV,
	SOL_BC_LDIV,
	SOL_BC_FDIV,
	SOL_BC_DDIV,
	// remainder
	SOL_BC// rem reg1 reg2 result_reg1 result_reg2
	SOL_BC_IREM,
	SOL_BC_LREM,
	SOL_BC_FREM,
	SOL_BC_DREM,
	// negate
	// neg from to
	SOL_BC_INEG,
	SOL_BC_LNEG,
	SOL_BC_FNEG,
	SOL_BC_DNEG,
	SOL_BC// and reg1 reg2 result_reg
	SOL_BC_IAND,
	SOL_BC_LAND,
	SOL_BC_FAND,
	SOL_BC_DAND,
	// or
	SOL_BC_IOR,
	SOL_BC_LOR,
	SOL_BC_FOR,
	SOL_BC_DOR,

	SOL_BC_IXOR,
	SOL_BC_LXOR,

	SOL_BC_ISHL,
	SOL_BC_LSHL,

	SOL_BC_IUSHL,
	SOL_BC_LUSHL,

	SOL_BC_ISHR,
	SOL_BC_LSHR,

	SOL_BC_IUSHR,
	SOL_BC_LUSHR,
	
	// compare
	// cmp reg1 reg2 result_reg
	SOL_BC_IEQ,
	SOL_BC_LEQ,
	SOL_BC_FEQ,
	SOL_BC_DEQ,

	SOL_BC_INEQ,
	SOL_BC_LNEQ,
	SOL_BC_FNEQ,
	SOL_BC_DNEQ,

	SOL_BC_IEQZ,
	SOL_BC_LEQZ,
	SOL_BC_FEQZ,
	SOL_BC_DEQZ,

	SOL_BC_IGT,
	SOL_BC_LGT,
	SOL_BC_FGT,
	SOL_BC_DGT,

	SOL_BC_ILT,
	SOL_BC_LLT,
	SOL_BC_FLT,
	SOL_BC_DLT,

	SOL_BC_JMP,
	SOL_BC_JSR,

	SOL_BC_INVOKE,

	SOL_BC_IRETURN,
	SOL_BC_LRETURN,
	SOL_BC_FRETURN,
	SOL_BC_DRETURN,
	SOL_BC_ARETURN,

	SOL_BC_HALT,

	SOL_BC_BREAKPOINT,
	SOL_BC_NUMBER_OF_OPCODES,
}

#endif
