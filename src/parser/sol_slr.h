#ifndef _SOL_SLR_
#define _SOL_SLR_ 1

#include <stdarg.h>
#include "sol_common.h"
#include "sol_list.h"
#include "sol_stack.h"
#include "sol_rbtree.h"
#include "sol_rb_tuple.h"

#define SolSLRParserItemCol_INIT_SIZE 32

#define SolLRSymbolFlag_ORIGIN       0x1
#define SolLRSymbolFlag_START        0x2
#define SolLRSymbolFlag_END          0x4
#define SolLRSymbolFlag_TERMINAL     0x8
#define SolLRSymbolFlag_NONTERMINAL  0x10
#define SolLRSymbolFlag_NULL         0x20
#define SolLRSymbolFlag_NULLABLE     0x30
#define SolLRSymbolFlag_COMPUTING    0x100
#define SolLRSymbolFlag_NULLABLE_COMPUTED    0x200
#define SolLRSymbolFlag_FIRST_COMPUTED       0x300

#define SolLRItemCol_FLAG_END  0x1
// action list
#define SolLRTableFieldFlag_ACTION_ACCEPT 0x1
#define SolLRTableFieldFlag_ACTION_GOTO   0x2
#define SolLRTableFieldFlag_ACTION_SHIFT  0x4
#define SolLRTableFieldFlag_ACTION_REDUCE 0x8
#define SolLRTableFieldFlag_TYPE_STATE    0x10
#define SolLRTableFieldFlag_TYPE_SYMBOL   0x20

typedef struct _SolLRSymbol {
    int f;   // flag
    void *s; // symbol
    SolList *p; // productions
	SolRBTree *firsts;
	SolRBTree *follows;
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
	SolLRSymbol *e; // empty symbol
    SolRBTuple *table; // parser table
} SolSLRParser;

struct _SolSLRTableField {
    int flag; // flag
    void *t; // taget
};

SolSLRParser* solSLRParser_new();
void solSLRParser_free(SolSLRParser*);

int solSLRParser_prepare(SolSLRParser*);

int solSLRParser_generate_items_collection();
int solSLRParser_compute_items_collections(SolSLRParser*, SolLRItemCol*);
int solSLRParser_compute_nonkernel_items(SolSLRParser*, SolLRItemCol*, SolLRSymbol*);

int solSLRParser_compute_parsing_table(SolSLRParser*);
// state1  read symbol -> do action -> state2
// solSLRParser_record_state_switch(SolSLRParser*, size_t, SolLRSymbol, action, size_t);
// state1 -> symbol1 -> action1 state2
// state1 -> symbol2 -> action1 state3
// state2 -> symbol3 -> action2 state3
// state3 -> symbol4 -> action3
int solSLRParser_record_accept(SolSLRParser*, size_t);
int solSLRParser_record_reduce(SolSLRParser*, size_t, SolLRSymbol*);
int solSLRParser_record_shift(SolSLRParser*, size_t, SolLRSymbol*, size_t);
int solSLRParser_record_goto(SolSLRParser*, size_t, SolLRSymbol*, size_t);

void solLRItemCol_free(SolLRItemCol*);

SolLRItem* solLRItem_new(SolLRProduct*, size_t, char);
void solLRItem_free(SolLRItem*);

SolLRProduct* solLRProduct_new(size_t, SolLRSymbol*, ...);
void solLRProduct_free(SolLRProduct*);

SolLRItemCol* solSLRParser_find_items_collection(SolSLRParser*, size_t);

int solLRSymbol_compute_nullable(SolLRSymbol*);
int solSLRParser_compute_first(SolSLRParser*, SolSymbol*);
int solSLRParser_compute_follow(SolSLRParser*, SolLRSymbol*);

int solLRSymbol_record_first(SolLRSymbol*, SolLRSymbol*);
int solLRSymbol_record_follow(SolLRSymbol*, SolLRSymbol*);
int solLRSymbol_share_firsts(SolRBTree*, SolRBTreeNode*, void*);
int solLRSymbol_share_follows(SolRBTree*, SolRBTreeNode*, void*);

#define solSLRParser_generate_state(p) (++((p)->gen))

#define solLRSymbol_is_terminal(s)   ((s)->flag & SolLRSymbolFlag_TERMINAL != 0)
#define solLRSymbol_is_nonterminal(s) ((s)->flag & SolLRSymbolFlag_NONTERMINAL != 0)
#define solLRSymbol_is_origin(s)   ((s)->flag & SolLRSymbolFlag_ORIGIN != 0)
#define solLRSymbol_is_computing(s) ((s)->flag & SolLRSymbolFlag_COMPUTING != 0)
#define solLRSymbol_is_idle(s) ((s)->flag & SolLRSymbolFlag_COMPUTING == 0)
#define solLRSymbol_set_busy(s) (s)->flag |= SolLRSymbolFlag_COMPUTING
#define solLRSymbol_set_is_idle(s) (s)->flag = ((s)->flag) & (~SolLRSymbolFlag_COMPUTING)
#define solLRSymbol_is_nullable(s) ((s)->flag & SolLRSymbolFlag_NULLABLE != 0)
#define solLRSymbol_set_nullable(s) ((s)->flag |= SolLRSymbolFlag_NULLABLE)
#define solLRSymbol_set_nullable_computed(s) ((s)->flag |= SolLRSymbolFlag_NULLABLE_COMPUTED)
#define solLRSymbol_nullable_computed(s) ((s)->flag & SolLRSymbolFlag_NULLABLE_COMPUTED != 0)
#define solLRSymbol_set_first_computed(s) ((s)->flag |= SolLRSymbolFlag_FIRST_COMPUTED)
#define solLRSymbol_first_computed(s) ((s)->flag & SolLRSymbolFlag_FIRST_COMPUTED != 0)

#define solLRSymbol_copy_firsts(f, symbol) solRBTree_travelsal_inorder(f, \
																	   solRBTree_root(f), \
																	   &solLRSymbol_share_firsts, \
																	   symbol)
#define solLRSymbol_copy_follows(f, symbol) solRBTree_travelsal_inorder(f, \
																		solRBTree_root(f), \
																		&solLRSymbol_share_follows, \
																		symbol)

#define solLRProduct_left(p) (p)->s
#define solLRProduct_right(p) (p)->r
#define solLRProduct_size(p) (p)->l
#define solLRProduct_find_symbol(p, s)  (SolLRSymbol*)((p)->r + s)

#endif
