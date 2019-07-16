#ifndef _SOL_VM_BYTECODE_ASSEMBLER_H_
#define _SOL_VM_BYTECODE_ASSEMBLER_H_ 1

#include "solType.h"
/**
 * assembler read asm code, convert to byte code
 */

#define SOL_VM_BC_ASM_INITIAL_CODE_SIZE 1024

typedef struct {
	SolObject constPool[SOL_VM_BC_ASM_INITIAL_CODE_SIZE];
	SolByte code[SOL_VM_BC_ASM_INITIAL_CODE_SIZE];
	size_t codeSize;
	SolFuncPtr enterFunc;
} SolVMBytecodeStorage;

typedef struct {
	int ip;
	int flag;
	SolVMBytecodeParser *parser;
	SolVMBytecodeStorage *storage;
} SolVMBytecodeAssembler;

int solVMBytecodeAssember_assemble(SolVMBytecodeAssembler*, SolVMTokenStream*);
int solVMBytecodeAssember_getInt();

#endif
