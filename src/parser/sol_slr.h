#ifndef _SOL_SLR_
#define _SOL_SLR_ 1

#include "sol_common.h"
#include "sol_list.h"
#include "sol_stack.h"
#include "sol_rbtree.h"

#define SolLRSymbolFlag_ORIGIN       0x1
#define SolLRSymbolFlag_START        0x2
#define SolLRSymbolFlag_TERMINAL     0x4
#define SolLRSymbolFlag_NONTERMINAL  0x8
#define SolLRSymbolFlag_NULL         0x16

typedef struct _SolLRSymbol {
    int f;   // flag
    void *s; // symbol
    SolList *i; // items
} SolLRSymbol;

typedef struct _SolLRProduct {
    size_t l; // len
    SolLRSymbol *s;
    SolLRSymbol *r; // right
} SolLRProduct;

typedef struct _SolLRItem {
    size_t step;
    SolLRProduct *s;
} SolLRItem;

typedef struct _SolSLRParser {
    int st; // state
    SolStack *s;
    SolList *p; // product list
    SolRBTree *ss; // symbols
} SolSLRParser;

SolSLRParser* solSLRParser_new();
void solSLRParser_free(SolSLRParser*);

int solSLRParser_generate_items(SolSLRParser*);
int solSLRParser_generate_goto(SolSLRParser*);

SolLRProduct* solLRProduct_new();
void solLRProduct_free(SolLRProduct*);

SolLRSymbol* solLRSymbol_new();
void solLRSymbol_free(SolLRSymbol*);
int solLRSymbol_add_item(SolLRSymbol*, SolLRProduct*, size_t);
int solLRSymbol_add_nonterminal_items(SolLRSymbol*, SolLRSymbol*);   // add items of symbol

SolLRItem* solLRItem_new(SolLRProduct*, int);
void solLRItem_free(SolLRItem*);

#define solLRSymbol_is_terminal(s)   ((s)->flag & SolLRSymbolFlag_TERMINAL > 0)
#define solLRSymbol_is_nonterminal(s) ((s)->flag & SolLRSymbolFlag_NONTERMINAL > 0)

#define solLRProduct_left(p) (p)->s
#define solLRProduct_right(p) (p)->r
#define solLRProduct_len(p) (p)->l
#define solLRProduct_find_symbol(p, s)  (SolLRSymbol*)((p)->r + s)

#endif
