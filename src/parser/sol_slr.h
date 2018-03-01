#ifndef _SOL_SLR_
#define _SOL_SLR_ 1

#include <stdarg.h>
#include "sol_common.h"
#include "sol_lr.h"
#include "sol_stack.h"
#include "sol_rb_tuple.h"

typedef struct _SolSLRParser {
    SolLRParser *lr;
    size_t state; // current state
    SolStack *stk; // stack
    SolRBTree *symbols; // symbols
    SolRBTuple *table; // parser table
} SolSLRParser;

typedef struct _SolSLRTableField {
    int flag; // flag
    void *t; // target
} SolSLRTableField;

SolSLRParser* solSLRParser_new();
void solSLRParser_free(SolSLRParser*);

SolLRSymbol* solSLRParser_terminal_new(SolSLRParser*, void*);
SolLRSymbol* solSLRParser_nonterminal_new(SolSLRParser*, void*);
int solSLRParser_regiter_symbol(SolSLRParser*, SolLRSymbol*);

int solSLRParser_prepare(SolSLRParser*);

SolLRItemCol* solSLRParser_generate_items_collection(SolSLRParser*);
SolLRItemCol* _solSLRParser_generate_items_collection(void*);

int solSLRParser_set_begin_product(SolSLRParser*, SolLRProduct*);
//int solSLRParser_compute_items_collections(SolSLRParser*, SolLRItemCol*);
//int solSLRParser_compute_nonkernel_items(SolSLRParser*, SolLRItemCol*, SolLRSymbol*);

int solSLRParser_compute_parsing_table(SolSLRParser*);
// state1  read symbol -> do action -> state2
// solSLRParser_record_state_switch(SolSLRParser*, size_t, SolLRSymbol, action, size_t);
// state1 -> symbol1 -> action1 state2
// state1 -> symbol2 -> action1 state3
// state2 -> symbol3 -> action2 state3
// state3 -> symbol4 -> action3

SolLRItemCol* solSLRParser_find_items_collection(SolSLRParser*, size_t);

int _solSLRParser_compare_symbols(void*, void*, SolRBTree*, int);
void _solSLRSymbol_free(void*);

int solSLRParser_record_accept(SolSLRParser*, SolLRTableField*);
int solSLRParser_record_reduce(SolSLRParser*, SolLRTableField*, SolLRTableField*);
int solSLRParser_record_shift(SolSLRParser*, SolLRTableField*, SolLRTableField*, SolLRTableField*);
int solSLRParser_record_goto(SolSLRParser*, SolLRTableField*, SolLRTableField*, SolLRTableField*);

int solSLRParser_record(SolSLRParser*, SolList*);
int _solSLRParser_record(SolList*, SolRBTuple*, void*);

#define solSLRParser_generate_state(p) (++((p)->gen))
#define solSLRParser_set_compare_symbol_val_func(p, f) p->lr->f_compare_symbol_val = f

#endif
