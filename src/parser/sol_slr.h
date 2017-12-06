#ifndef _SOL_SLR_
#define _SOL_SLR_ 1

#include "sol_common.h"
#include "sol_list.h"
#include "sol_stack.h"
#include "sol_rbtree.h"

#define SolLRSymbolFlag_ORIGIN       0x1
#define SolLRSymbolFlag_START        0x2
#define SolLRSymbolFlag_END          0x4
#define SolLRSymbolFlag_TERMINAL     0x8
#define SolLRSymbolFlag_NONTERMINAL  0x10
#define SolLRSymbolFlag_NULL         0x20

typedef struct _SolLRSymbol {
    int f;   // flag
    void *s; // symbol
    SolList *p; // productions
} SolLRSymbol;

typedef struct _SolLRProduct {
    size_t s; // size
    SolLRSymbol *s;
    SolLRSymbol *r; // right
} SolLRProduct;

typedef struct _SolLRItem {
    size_t pos;
    SolLRProduct *p;
} SolLRItem;

typedef struct _SolLRItemCol { // items collection
    size_t s; // state
    SolLRItem *is; // items
} SolLRCol;

typedef struct _SolLRGoto {
    size_t pos;
    int cs; // current state
    int ns; // next state
    SolLRSymbol *s;
    SolLRProduct *p;
} SolLRGoto;

typedef struct _SolSLRParser {
    size_t ig; // state generate
    size_t ics; // items collection size
    SolStack *_stk; // stack
    SolRBTree *_nts; // nonterminals
    SolList *_ts; // terminals
    SolLRItemCol *_ic; // items collection
    SolLRGoto *_gt; // goto list
    SolLRSymbol *s; // start symbol
} SolSLRParser;

SolSLRParser* solSLRParser_new();
void solSLRParser_free(SolSLRParser*);

int solSLRParser_generate_items(SolSLRParser*);
int solSLRParser_generate_goto(SolSLRParser*);

SolLRProduct* solLRProduct_new();
void solLRProduct_free(SolLRProduct*);

#define solSLRParser_generate_state(p) (++((p)->ig))

#define solLRSymbol_is_terminal(s)   ((s)->flag & SolLRSymbolFlag_TERMINAL > 0)
#define solLRSymbol_is_nonterminal(s) ((s)->flag & SolLRSymbolFlag_NONTERMINAL > 0)
#define solLRSymbol_is_origin(s)   ((s)->flag & SolLRSymbolFlag_ORIGIN > 0)

#define solLRProduct_left(p) (p)->s
#define solLRProduct_right(p) (p)->r
#define solLRProduct_size(p) (p)->l
#define solLRProduct_find_symbol(p, s)  (SolLRSymbol*)((p)->r + s)

#endif
