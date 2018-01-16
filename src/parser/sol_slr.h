#ifndef _SOL_SLR_
#define _SOL_SLR_ 1

#include <stdarg.h>
#include "sol_common.h"
#include "sol_lr.h"
#include "sol_stack.h"
#include "sol_rb_tuple.h"

// action list
#define SolLRTableFieldFlag_ACTION_ACCEPT 0x1
#define SolLRTableFieldFlag_ACTION_GOTO   0x2
#define SolLRTableFieldFlag_ACTION_SHIFT  0x4
#define SolLRTableFieldFlag_ACTION_REDUCE 0x8
#define SolLRTableFieldFlag_TYPE_STATE    0x10
#define SolLRTableFieldFlag_TYPE_SYMBOL   0x20

typedef struct _SolSLRParser {
    SolLRParser *lr;
    size_t state; // current state
    SolStack *stk; // stack
    SolRBTree *symbols; // symbols
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

int _solSLRParserField_compare(void*, void*);
void _solSLRParserField_free(void*);

int solSLRParser_record_accept(SolSLRParser*, SolLRItemCol*);
int solSLRParser_record_reduce(SolSLRParser*, SolLRItemCol*, SolLRSymbol*);
int solSLRParser_record_shift(SolSLRParser*, SolLRItemCol*, SolLRSymbol*, SolLRItemCol*);
int solSLRParser_record_goto(SolSLRParser*, SolLRItemCol*, SolLRSymbol*, SolLRItemCol*);

int _solSLRSymbol_compare(void*, void*);

#define solSLRParser_generate_state(p) (++((p)->gen))

#endif
