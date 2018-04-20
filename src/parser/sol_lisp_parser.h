#ifndef _SOL_LISP_PARSER_H_
#define _SOL_LISP_PARSER_H_ 1

#include "sol_common.h"
#include "sol_pda.h"

#define SolLispParserSymbolType_char 0x1

typedef struct _SolLispParserChar {
	void *c;
	size_t len;
} SolLispParserChar;

typedef struct _SolLispParser {
	SolPda *pda;
	size_t (*read_char)(char*, size_t);
	int flag;
} SolLispParser;

SolLispParser* solLispParser_new();
void solLispParser_free(SolLispParser*);

#endif
