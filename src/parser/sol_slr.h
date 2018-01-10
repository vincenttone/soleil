#ifndef _SOL_SLR_
#define _SOL_SLR_ 1

#include <stdarg.h>
#include "sol_common.h"
#include "sol_stack.h"
#include "sol_rb_tuple.h"

#define SolSLRParserItemCol_INIT_SIZE 32

#define SolLRItemCol_FLAG_END  0x1
// action list
#define SolLRTableFieldFlag_ACTION_ACCEPT 0x1
#define SolLRTableFieldFlag_ACTION_GOTO   0x2
#define SolLRTableFieldFlag_ACTION_SHIFT  0x4
#define SolLRTableFieldFlag_ACTION_REDUCE 0x8
#define SolLRTableFieldFlag_TYPE_STATE    0x10
#define SolLRTableFieldFlag_TYPE_SYMBOL   0x20

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

SolLRItemCol* solSLRParser_find_items_collection(SolSLRParser*, size_t);

int solLRSymbol_compute_nullable(SolLRSymbol*);
int solLRSymbol_compute_first(SolSLRParser*, SolSymbol*, SolSymbol*);
int solSLRParser_compute_follow(SolSLRParser*, SolLRSymbol*);

int _solSLRParserField_compare(void*, void*);
void _solSLRParserField_free(void*);

int solSLRParser_record_accept(SolSLRParser*, size_t);
int solSLRParser_record_reduce(SolSLRParser*, size_t, SolLRSymbol*);
int solSLRParser_record_shift(SolSLRParser*, size_t, SolLRSymbol*, size_t);
int solSLRParser_record_goto(SolSLRParser*, size_t, SolLRSymbol*, size_t);

#define solSLRParser_generate_state(p) (++((p)->gen))

#endif
