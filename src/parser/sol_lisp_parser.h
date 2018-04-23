#ifndef _SOL_LISP_PARSER_H_
#define _SOL_LISP_PARSER_H_ 1

#include "sol_common.h"
#include "sol_pda.h"

enum SolLispParserSymbolVal {
	SolLispParserSymbolVal_lc = 1,
	SolLispParserSymbolVal_rc,
	SolLispParserSymbolVal_blank,
	SolLispParserSymbolVal_other,
	SolLispParserSymbolVal_max,
};

typedef struct _SolLispParserSymbol {
	int s;
} SolLispParserSymbol;

typedef struct _SolLispParser {
	SolPda *pda;
	size_t (*read_char)(char*, size_t);
	enum SolLispParserSymbolVal (*get_symbol_val)(char*, size_t);
	SolLispParserSymbol *symbols;
	int flag;
} SolLispParser;

SolLispParser* solLispParser_new();
int solLispParser_read(SolLispParser*, char*, size_t);
void solLispParser_free(SolLispParser*);

size_t solLispParser_symbol_hash1(void*);
size_t solLispParser_symbol_hash2(void*);
int solLispParser_symbol_hash_equal(void*, void*);

#endif
