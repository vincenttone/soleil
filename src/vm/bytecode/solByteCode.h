#ifndef _SOL_BYTECODES_
#define _SOL_BYTECODES_ 1

enum SolByteCode {
	_breakpoint = -2,
	_illegal = -1,
	
	_nop     = 0,

	// COMMAND FROM TO
	// load immediate number into register
	// const imm $reg
	_iconst,
	_lconst,
	_fconst,
	_dconst,
	// load from local variable into register
	// load addr $reg
	_iload,
	_lload,
	_fload,
	_dload,
	// store from register into local variable
	// store $reg addr
	_istore,
	_lstore,
	_fstore,
	_dstore,

	// duplication
	// dup reg1 reg2
	_dup,
	_swap,
	_push,
	_pop,

	// type convertion
	_d2f,
	_d2i,
	_d2l,

	_f2d,
	_f2i,
	_f2l,

	_i2d,
	_i2f,
	_i2l,

	_l2d,
	_l2f,
	_l2i,

	// computation
	_iadd,
	_ladd,
	_fadd,
	_dadd,

	_isub,
	_lsub,
	_fsub,
	_dsub,

	_imul,
	_lmul,
	_fmul,
	_dmul,

	_idiv,
	_ldiv,
	_fdiv,
	_ddiv,
	// remainder
	// rem reg1 reg2 result_reg1 result_reg2
	_irem,
	_lrem,
	_frem,
	_drem,
	// negate
	// neg from to
	_ineg,
	_lneg,
	_fneg,
	_dneg,
	// and reg1 reg2 result_reg
	_iand,
	_land,
	_fand,
	_dand,
	// or
	_ior,
	_lor,
	_for,
	_dor,

	_ixor,
	_lxor,

	_ishl,
	_lshl,

	_iushl,
	_lushl,

	_ishr,
	_lshr,

	_iushr,
	_lushr,
	
	// compare
	// cmp reg1 reg2 result_reg
	_ieq,
	_leq,
	_feq,
	_deq,

	_ineq,
	_lneq,
	_fneq,
	_dneq,

	_ieqz,
	_leqz,
	_feqz,
	_deqz,

	_igt,
	_lgt,
	_fgt,
	_dgt,

	_ilt,
	_llt,
	_flt,
	_dlt,

	_jmp,
	_jsr,

	_invoke,

	_ireturn,
	_lreturn,
	_freturn,
	_dreturn,
	_areturn,

	_number_of_opcodes,
}

#endif
