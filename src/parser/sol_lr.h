#ifndef _SOL_LR_
#define _SOL_LR_ 1

#include <stdarg.h>
#include "sol_common.h"
#include "sol_rbtree.h"
#include "sol_list.h"

#define SolLRSymbolFlag_ORIGIN       0x1
#define SolLRSymbolFlag_START        0x2
#define SolLRSymbolFlag_END          0x4
#define SolLRSymbolFlag_TERMINAL     0x8
#define SolLRSymbolFlag_NONTERMINAL  0x10
#define SolLRSymbolFlag_EMPTY        0x20
#define SolLRSymbolFlag_NULLABLE     0x30
#define SolLRSymbolFlag_COMPUTING    0x100
#define SolLRSymbolFlag_NULLABLE_COMPUTED    0x200
#define SolLRSymbolFlag_FIRST_COMPUTED       0x300

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

int solLRSymbol_new(void*);
void solLRSymbol_free(SolLRSymbol*);

SolLRProduct* solLRProduct_new(size_t, SolLRSymbol*, ...);
void solLRProduct_free(SolLRProduct*);

SolLRItem* solLRItem_new(SolLRProduct*, size_t, char);
void solLRItem_free(SolLRItem*);

SolLRItemCol* solLRItemCol_new();
void solLRItemCol_free(SolLRItemCol*);

int solLRSymbol_record_first(SolLRSymbol*, SolLRSymbol*);
int solLRSymbol_record_follow(SolLRSymbol*, SolLRSymbol*);

int _solLRSymbol_share_firsts(SolRBTree*, SolRBTreeNode*, void*);
int _solLRSymbol_share_follows(SolRBTree*, SolRBTreeNode*, void*);

#define solLRSymbol_is_terminal(s)   ((s)->flag & SolLRSymbolFlag_TERMINAL != 0)
#define solLRSymbol_is_nonterminal(s) ((s)->flag & SolLRSymbolFlag_NONTERMINAL != 0)
#define solLRSymbol_is_origin(s)   ((s)->flag & SolLRSymbolFlag_ORIGIN != 0)
#define solLRSymbol_is_nullable(s) ((s)->flag & SolLRSymbolFlag_NULLABLE != 0)
#define solLRSymbol_set_nullable(s) ((s)->flag |= SolLRSymbolFlag_NULLABLE)
#define solLRSymbol_set_nullable_computed(s) ((s)->flag |= SolLRSymbolFlag_NULLABLE_COMPUTED)
#define solLRSymbol_nullable_computed(s) ((s)->flag & SolLRSymbolFlag_NULLABLE_COMPUTED != 0)
#define solLRSymbol_set_first_computed(s) ((s)->flag |= SolLRSymbolFlag_FIRST_COMPUTED)
#define solLRSymbol_first_computed(s) ((s)->flag & SolLRSymbolFlag_FIRST_COMPUTED != 0)

#define solLRSymbol_is_computing(s) ((s)->flag & SolLRSymbolFlag_COMPUTING != 0)
#define solLRSymbol_is_idle(s) ((s)->flag & SolLRSymbolFlag_COMPUTING == 0)
#define solLRSymbol_set_busy(s) (s)->flag |= SolLRSymbolFlag_COMPUTING
#define solLRSymbol_set_is_idle(s) (s)->flag = ((s)->flag) & (~SolLRSymbolFlag_COMPUTING)

#define solLRSymbol_copy_firsts(f, symbol) solRBTree_travelsal_inorder(f, \
																	   solRBTree_root(f), \
																	   &_solLRSymbol_share_firsts, \
																	   symbol)
#define solLRSymbol_copy_follows(f, symbol) solRBTree_travelsal_inorder(f, \
																		solRBTree_root(f), \
																		&_solLRSymbol_share_follows, \
																		symbol)

#define solLRProduct_left(p) (p)->s
#define solLRProduct_right(p) (p)->r
#define solLRProduct_size(p) (p)->l
#define solLRProduct_find_symbol(p, s)  (SolLRSymbol*)((p)->r + s)

#endif
