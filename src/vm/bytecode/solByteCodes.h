#ifndef _SOL_BYTECODES_
#define _SOL_BYTECODES_ 1

enum SolByteCodes {
	_illegal = -1,
	_nop     = 0,
	
	_move,

	_load,

	_store,

	_push,
	_pop,
	_dup,
	_swap,

	// _sadd, // short
	_iadd, // int
	_fadd, // float

	// _ssub,
	_isub,
	_fsub,

	_imul,
	_fmul,

	_idiv,
	_fdiv,

	_irem,
	_frem,

	_ineg,
	_fneg,

	_shl,

	_shr,

	_ushr,

	_and,

	_or,

	_xor,

	_iinc,

	_ifeq,
	_ifne,
	_iflt,
	_ifge,
	_ifgt,
	_ifle,

	_goto,
	_jsr,
	_call,
	_ret,

	_trap,
	_eret,


	number_of_codes,
}

#endif
