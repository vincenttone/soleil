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
	size_t len;
	int s;
	char *buffer;
} SolLispParserSymbol;

typedef struct _SolLispParserAstNode {
	size_t len;
	char *val;
	struct _SolLispParserAstNode *parent;
	SolList *children;
} SolLispParserAstNode;

typedef struct _SolLispParser {
	int _c;
	SolPda *pda;
	SolLispParserSymbol *symbols;
	SolLispParserAstNode *ast;
	SolLispParserAstNode *can; // current ast node
	size_t (*read_char)(char*, size_t);
	enum SolLispParserSymbolVal (*get_symbol_val)(char*, size_t);
	int (*block_begin)(struct _SolLispParser*, SolLispParserSymbol*);
	int (*block_end)(struct _SolLispParser*, SolLispParserSymbol*);
	int (*element_next)(struct _SolLispParser*, SolLispParserSymbol*);
	int (*element)(struct _SolLispParser*, SolLispParserSymbol*);
} SolLispParser;

SolLispParser* solLispParser_new();
int solLispParser_read(SolLispParser*, char*, size_t);
void solLispParser_free(SolLispParser*);

SolLispParserAstNode* solLispParserAstNode_new(char*, size_t, SolLispParserAstNode*);
void solLispParserAstNode_free(SolLispParserAstNode*);

size_t solLispParser_symbol_hash1(void*);
size_t solLispParser_symbol_hash2(void*);
int solLispParser_symbol_hash_equal(void*, void*);

int solLispParser_block_begin(void*, void*);
int solLispParser_block_end(void*, void*);
int solLispParser_element_next(void*, void*);
int solLispParser_element(void*, void*);

#endif
