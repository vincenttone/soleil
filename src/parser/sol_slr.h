#ifndef _SOL_SLR_
#define _SOL_SLR_ 1

#include <stdarg.h>
#include "sol_common.h"
#include "sol_list.h"
#include "sol_stack.h"
#include "sol_rbtree.h"

#define SolSLRParserItemCol_INIT_SIZE 32

#define SolLRSymbolFlag_ORIGIN       0x1
#define SolLRSymbolFlag_START        0x2
#define SolLRSymbolFlag_END          0x4
#define SolLRSymbolFlag_TERMINAL     0x8
#define SolLRSymbolFlag_NONTERMINAL  0x10
#define SolLRSymbolFlag_NULL         0x20
#define SolLRSymbolFlag_COMPUTING    0x100

#define SolLRItem_KERNEL    0
#define SolLRItem_NONKERNEL 1

#define SolLRItemCol_FLAG_END  0x1

typedef struct _SolLRSymbol {
    int f;   // flag
    void *s; // symbol
    SolList *p; // productions
} SolLRSymbol;

typedef struct _SolLRProduct {
    size_t len; // len
    SolLRSymbol *s;
    SolLRSymbol *r; // right
} SolLRProduct;

typedef struct _SolLRItem {
    size_t pos;
    SolLRProduct *p;
} SolLRItem;

typedef struct _SolLRItemCol { // items collection
    size_t state; // state
    size_t flag; // flag
    SolList *items; // items
    SolRBTree *nc; // next items collection
    SolLRSymbol *sym; // pre symbol
} SolLRItemCol;

typedef struct _SolSLRParser {
    size_t state; // current state
    size_t gen; // state generate
    size_t size_col; // items collection size
    SolStack *stk; // stack
    SolRBTree *symbols; // symbols
    SolLRItemCol *collections; // items collection
    SolLRSymbol *s; // start symbol
} SolSLRParser;

SolSLRParser* solSLRParser_new();
void solSLRParser_free(SolSLRParser*);
int solSLRParser_prepare(SolSLRParser*);

int solSLRParser_generate_items_collection();
int solSLRParser_compute_items_collections(SolSLRParser*, SolLRItemCol*);
int solSLRParser_add_to_goto(SolSLRParser*, size_t, SolLRSymbol*, size_t);

void solLRItemCol_free(SolLRItemCol*);

SolLRItem* solLRItem_new(SolLRProduct*, size_t, char);
void solLRItem_free(SolLRItem*);

SolLRProduct* solLRProduct_new(size_t, SolLRSymbol*, ...);
void solLRProduct_free(SolLRProduct*);

#define solSLRParser_generate_state(p) (++((p)->gen))

#define solLRSymbol_is_terminal(s)   ((s)->flag & SolLRSymbolFlag_TERMINAL != 0)
#define solLRSymbol_is_nonterminal(s) ((s)->flag & SolLRSymbolFlag_NONTERMINAL != 0)
#define solLRSymbol_is_origin(s)   ((s)->flag & SolLRSymbolFlag_ORIGIN != 0)
#define solLRSymbol_is_computing(s) ((s)->flag & SolLRSymbolFlag_COMPUTING != 0)
#define solLRSymbol_is_idle(s) ((s)->flag & SolLRSymbolFlag_COMPUTING == 0)
#define solLRSymbol_set_busy(s) (s)->flag |= SolLRSymbolFlag_COMPUTING
#define solLRSymbol_set_is_idle(s) (s)->flag = ((s)->flag) & (~SolLRSymbolFlag_COMPUTING)

#define solLRProduct_left(p) (p)->s
#define solLRProduct_right(p) (p)->r
#define solLRProduct_size(p) (p)->l
#define solLRProduct_find_symbol(p, s)  (SolLRSymbol*)((p)->r + s)

#endif
