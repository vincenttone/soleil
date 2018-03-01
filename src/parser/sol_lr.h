#ifndef _SOL_LR_
#define _SOL_LR_ 1

#include <stdarg.h>
#include "sol_common.h"
#include "sol_rbtree.h"
#include "sol_list.h"
#include "sol_stack.h"
#include "sol_rb_tuple.h"

#define SolLRSymbolFlag_ORIGIN               0x1
#define SolLRSymbolFlag_START                0x2
#define SolLRSymbolFlag_END                  0x4
#define SolLRSymbolFlag_TERMINAL             0x10
#define SolLRSymbolFlag_NONTERMINAL          0x20
#define SolLRSymbolFlag_EMPTY                0x40
#define SolLRSymbolFlag_NULLABLE             0x50
#define SolLRSymbolFlag_COMPUTING            0x100
#define SolLRSymbolFlag_NULLABLE_COMPUTED    0x200
#define SolLRSymbolFlag_FIRST_COMPUTED       0x300
#define SolLRSymbolFlag_FOLLOW_COMPUTED      0x400

#define SolLRItemCol_FLAG_KERNEL_COMPUTED    0x1
#define SolLRItemCol_FLAG_RECORDED           0x2
#define SolLRItemCol_FLAG_NONKERNEL_COMPUTED 0x4
#define SolLRItemCol_FLAG_END                0x8

// action list
#define SolLRTableFieldFlag_ACTION_ACCEPT           0x1
#define SolLRTableFieldFlag_ACTION_GOTO             0x2
#define SolLRTableFieldFlag_ACTION_SHIFT            0x4
#define SolLRTableFieldFlag_ACTION_REDUCE           0x8
#define SolLRTableFieldFlag_TYPE_STATE              0x10
#define SolLRTableFieldFlag_TYPE_COL                0x10
#define SolLRTableFieldFlag_TYPE_SYMBOL             0x20
#define SolLRTableFieldFlag_TYPE_PRODUCT            0x30
#define SolLRTableFieldFlag_COL_REPEATABLE          0x100

typedef struct _SolLRSymbol {
    int flag;   // flag
    void *v; // symbol val
    SolList *productions; // productions
    SolRBTree *firsts;
    SolRBTree *follows;
} SolLRSymbol;

typedef struct _SolLRProduct {
    size_t len; // len
    SolLRSymbol *s;
    SolLRSymbol **r; // right
    struct _SolLRItem *items; // items
} SolLRProduct;

typedef struct _SolLRItem {
    size_t pos;
    SolLRProduct *p;
    int flag;
    SolList *cols;
} SolLRItem;

typedef struct _SolLRItemCol { // items collection
    size_t state; // state
    size_t flag; // flag
    SolRBTree *items; // items
    SolLRSymbol *sym; // pre symbol
    SolList *nexts; // next cols
    SolList *ends; // item tail list
} SolLRItemCol;

typedef struct _SolLRParser {
    size_t gen; // state generate
    SolStack *stk;
    SolList *collections; // items collection
    SolRBTuple *col_rel; // items colletion relations
    SolList *fields;
    SolLRSymbol *origin; // start symbol
    SolLRSymbol *empty; // empty symbol
    SolLRSymbol *end; // end symbol
    int (*f_compare_symbol_val)(void*, void*);
#ifdef __SOL_DEBUG__
    void (*f_debug_symbol)(SolLRSymbol*, struct _SolLRParser*);
    void (*f_debug_product)(SolLRProduct*, struct _SolLRParser*);
    void (*f_debug_item)(SolLRItem*, struct _SolLRParser*);
    void (*f_debug_item_col)(SolLRItemCol*, struct _SolLRParser*);
#endif
} SolLRParser;

typedef struct _SolLRTableField {
    void *target;
    int flag;
} SolLRTableField;

SolLRParser* solLRParser_new();
void solLRParser_free(SolLRParser*);
SolLRItemCol* solLRParser_generate_items_collection(SolLRParser*, SolLRSymbol*, int);
int solLRParser_compare_symbol(SolLRParser*, SolLRSymbol*, SolLRSymbol*);

SolLRSymbol* solLRSymbol_new(void*);
void solLRSymbol_free(SolLRSymbol*);
void _solLRSymbol_free(void*);

SolLRSymbol* solLRSymbol_terminal_new(void*);
SolLRSymbol* solLRSymbol_nonterminal_new(void*);

SolLRProduct* solLRProduct_new(SolLRSymbol*, size_t, ...);
void solLRProduct_free(SolLRProduct*);
void _solLRProduct_free(void*);

SolLRItem* solLRProduct_item(SolLRProduct*, size_t);

SolLRItemCol* solLRParser_itemCol_new(SolLRParser*, SolLRSymbol*);
SolLRItemCol* solLRItemCol_new(SolLRSymbol*);
void solLRItemCol_free(SolLRItemCol*);
void _solLRItemCol_free(void*);

int _solLRItemsCols_collect_item(SolRBTree*, SolRBTreeNode*, void*);
int solLRParser_collect_kernel_item(SolLRParser*, SolLRItemCol*, SolLRItem*);

SolLRTableField* solLRParserTableField_new(SolLRParser*, void*, int);
SolLRTableField* solLRParserTableField_clone(SolLRParser*, SolLRTableField*, int);
int solLRParserField_compare(SolLRParser*, SolLRTableField*, SolLRTableField*);
void solLRParserTableField_free(SolLRTableField*);

int solLRSymbol_record_first(SolLRSymbol*, SolLRSymbol*, SolLRParser*);
int solLRSymbol_record_follow(SolLRSymbol*, SolLRSymbol*, SolLRParser*);
SolLRItemCol* solLRParser_record_items_collection_relations(SolLRParser*, SolLRItemCol*, SolLRItemCol*);

int solLRItem_compare(SolLRItem*, SolLRItem*);

int _solLRSymbol_share_firsts(SolRBTree*, SolRBTreeNode*, void*);
int _solLRSymbol_share_follows(SolRBTree*, SolRBTreeNode*, void*);

int _solLRParser_compare_symbols(void*, void*, SolRBTree*, int);
int _solLRParserField_compare(void*, void*, SolRBTuple*, int);
int _solLRItem_compare(void*, void*, SolRBTree*, int);

int _solLRItem_register_collection(SolRBTree*, SolRBTreeNode*, void*);

int solLRSymbol_compute_nullable(SolLRSymbol*);
int solLRSymbol_compute_first(SolLRSymbol*, SolLRSymbol*, SolLRParser*);
int solLRSymbol_compute_follow(SolLRSymbol*, SolRBTree*, SolLRSymbol*, SolLRParser*);

int solLRParser_compute_items_collections(SolLRParser*, SolLRItemCol*);
int solLRParser_collect_from_nonkernel_items(SolLRParser*, SolLRItemCol*, SolLRSymbol*);

#define solLRSymbol_set_flag(s, f) ((s)->flag |= f)

#define solLRSymbol_is_terminal(s)   ((s)->flag & SolLRSymbolFlag_TERMINAL)
#define solLRSymbol_is_nonterminal(s) ((s)->flag & SolLRSymbolFlag_NONTERMINAL)
#define solLRSymbol_is_origin(s)   ((s)->flag & SolLRSymbolFlag_ORIGIN)

#define solLRSymbol_is_nullable(s) ((s)->flag & SolLRSymbolFlag_NULLABLE)
#define solLRSymbol_set_nullable(s) ((s)->flag |= SolLRSymbolFlag_NULLABLE)

#define solLRSymbol_set_nullable_computed(s) ((s)->flag |= SolLRSymbolFlag_NULLABLE_COMPUTED)
#define solLRSymbol_nullable_computed(s) ((s)->flag & SolLRSymbolFlag_NULLABLE_COMPUTED)

#define solLRSymbol_set_first_computed(s) ((s)->flag |= SolLRSymbolFlag_FIRST_COMPUTED)
#define solLRSymbol_first_computed(s) ((s)->flag & SolLRSymbolFlag_FIRST_COMPUTED)

#define solLRSymbol_set_follow_computed(s) ((s)->flag |= SolLRSymbolFlag_FOLLOW_COMPUTED)
#define solLRSymbol_follow_computed(s) ((s)->flag & SolLRSymbolFlag_FOLLOW_COMPUTED)

#define solLRSymbol_is_computing(s) ((s)->flag & SolLRSymbolFlag_COMPUTING)
#define solLRSymbol_is_idle(s) (((s)->flag & SolLRSymbolFlag_COMPUTING) == 0)
#define solLRSymbol_set_is_busy(s) (s)->flag |= SolLRSymbolFlag_COMPUTING
#define solLRSymbol_set_is_idle(s) (s)->flag = ((s)->flag) & (~SolLRSymbolFlag_COMPUTING)

#define solLRSymbol_copy_firsts(f, symbol) solRBTree_travelsal_inorder(f, \
                                                                       solRBTree_root(f), \
                                                                       &_solLRSymbol_share_firsts, \
                                                                       symbol)
#define solLRSymbol_copy_follows(f, symbol) solRBTree_travelsal_inorder(f, \
                                                                        solRBTree_root(f), \
                                                                        &_solLRSymbol_share_follows, \
                                                                        symbol)

#define solLRProduct_find_symbol_ptr(p, s)  (SolLRSymbol**)((p)->r + s)
#define solLRProduct_find_symbol(p, s)  (*(SolLRSymbol**)((p)->r + s))

#endif
