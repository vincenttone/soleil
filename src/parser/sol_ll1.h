#ifndef _SOL_LL1_PARSER_H_
#define _SOL_LL1_PARSER_H_ 1

#include "sol_common.h"
#include "sol_list.h"
#include "sol_stack.h"

typedef enum _SolLL1ParserSignType {
    SolLL1ParserSignType_Noterminal = 1;
    SolLL1ParserSignType_Terminal;
} SolLL1ParserSignType;

typedef struct _SolLL1ParserSign {
    void *s;
    SolLL1ParserSignType *t;
} SolLL1ParserSign;

typedef struct _SolLL1ParserExp {
    SolLL1ParserSign *s;
    struct _SolLL1ParserExp *n;
} SolLL1ParserExp;

typedef struct _SolLL1ParserRule {
    SolLL1ParserSign *t;
    SolLL1ParserExp *e;
} SolLL1ParserRule;

typedef struct _SolLL1ParserTable {
} SolLL1ParserTable;

typedef struct _SolLL1Parser {
    SolLL1ParserTable *t;
    SolStack *s;
    SolList *el; // expression list
} SolLL1Parser;

SolLL1Parser* solLL1Parser_new();
void solLL1Parser_free();

SolLL1ParserRule* solLL1Parser_find(void*, void*);

SolLL1ParserTable* solLL1ParserTable_new();
void solLL1ParserTable_free();

#endif
