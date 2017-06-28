#ifndef _SOL_LL1_PARSER_H_
#define _SOL_LL1_PARSER_H_ 1

#include "sol_common.h"
#include "sol_list.h"
#include "sol_stack.h"
#include "sol_hash.h"

#define SolLL1ParserSymbolFlag_NULL 0x1
#define SolLL1ParserSymbolFlag_Terminal 0x2
#define SolLL1ParserSymbolFlag_Nonterminal 0x4
#define SolLL1ParserSymbolFlag_Computed_NULL 0x8
#define SolLL1ParserSymbolFlag_Computed_FIRST 0x10
#define SolLL1ParserSymbolFlag_Computed_FOLLOW 0x20

typedef struct _SolLL1ParserSymbol {
    void *s;
    int f;
    SolList *first;
    SolList *follow;
} SolLL1ParserSymbol;

#define SolLL1ParserForm SolList

typedef struct _SolLL1Parser {
    SolHash *t;
    SolStack *s;
    SolList *fl; // form list
    SolList *ss; // symbol list
} SolLL1Parser;

typedef struct _SolLL1ParserTableK {
    SolLL1ParserSymbol *s1;
    SolLL1ParserSymbol *s2;
} SolLL1ParserTableK;

SolLL1Parser* solLL1Parser_new();
void solLL1Parser_free(SolLL1Parser*);
int solLL1Parser_reg_form(SolLL1Parser*, SolLL1ParserForm*);
int solLL1Parser_reg_symbol(SolLL1Parser*, SolLL1ParserSymbol*);
SolLL1ParserSymbol* solLL1Parser_terminal(SolLL1Parser*, void*);
SolLL1ParserSymbol* solLL1Parser_nonterminal(SolLL1Parser*, void*);
SolLL1ParserSymbol* solLL1Parser_null(SolLL1Parser*, void*);

int solLL1Parser_generate_table(SolLL1Parser*);
int solLL1Parser_symbol_compute_first(SolLL1Parser*, SolLL1ParserSymbol*);
int solLL1Parser_symbol_compute_follow(SolLL1Parser*, SolLL1ParserSymbol*);
int solLL1Parser_symbol_compute_nullable(SolLL1Parser*, SolLL1ParserSymbol*);

#define solLL1ParserForm_new() solList_new()
#define solLL1ParserForm_free(f) solList_free(f)
int solLL1ParserForm_add_symbol(SolLL1ParserForm*, SolLL1ParserSymbol*);
void _solLL1ParserForm_free(void*);

SolLL1ParserSymbol* solLL1ParserSymbol_new(void*, int);
void solLL1ParserSymbol_free(SolLL1ParserSymbol*);
void _solLL1ParserSymbol_free(void*);
int solLL1ParserSymbol_add_first(SolLL1ParserSymbol*, SolLL1ParserSymbol*);
int solLL1ParserSymbol_add_follow(SolLL1ParserSymbol*, SolLL1ParserSymbol*);

int solLL1Parser_table_add_rule(SolLL1Parser*, SolLL1ParserSymbol*, SolLL1ParserSymbol*, SolLL1ParserForm*);

#define solLL1Parser_set_stack(p, stack) (p)->s = stack
#define solLL1Parser_set_table(p, table) (p)->t = table
#define solLL1Parser_set_form_list(p, l) (p)->fl = l
#define solLL1Parser_set_symbol_list(p, l) (p)->ss = l

#define solLL1Parser_stack(p) (p)->s
#define solLL1Parser_table(p) (p)->t
#define solLL1Parser_form_list(p) (p)->fl
#define solLL1Parser_symbol_list(p) (p)->ss

#define solLL1ParserForm_set_symbol(f, symbol) (f)->s = symbol
#define solLL1ParserForm_set_next(f, next) (f)->n = next

#define solLL1ParserForm_symbol(f) (f)->s
#define solLL1ParserForm_next(f) (f)->n

#define solLL1ParserSymbol_set_symbol(symbol, d) (symbol)->s = d
#define solLL1ParserSymbol_set_type(symbol, type) (symbol)->f = (symbol)->f | type
#define solLL1ParserSymbol_set_first(symbol, s) (symbol)->first = s
#define solLL1ParserSymbol_set_follow(symbol, s) (symbol)->follow = s

#define solLL1ParserSymbol_symbol(symbol) (symbol)->s
#define solLL1ParserSymbol_first(symbol) (symbol)->first
#define solLL1ParserSymbol_follow(symbol) (symbol)->follow

#define solLL1ParserSymbol_is_nonterminal(symbol) ((symbol)->f & SolLL1ParserSymbolFlag_Nonterminal)
#define solLL1ParserSymbol_is_terminal(symbol) ((symbol)->f & SolLL1ParserSymbolFlag_Terminal)
#define solLL1ParserSymbol_is_nullable(symbol) ((symbol)->f & SolLL1ParserSymbolFlag_NULL)
#define solLL1ParserSymbol_is_nullable_computed(symbol) ((symbol)->f & SolLL1ParserSymbolFlag_Computed_NULL)
#define solLL1ParserSymbol_is_first_computed(symbol) ((symbol)->f & SolLL1ParserSymbolFlag_Computed_FIRST)
#define solLL1ParserSymbol_is_follow_computed(symbol) ((symbol)->f & SolLL1ParserSymbolFlag_Computed_FOLLOW)

#define solLL1ParserSymbol_is_NOT_nonterminal(symbol) (((symbol)->f & SolLL1ParserSymbolFlag_Nonterminal) == 0)

#define solLL1ParserSymbol_set_nullable(symbol) (symbol)->f = ((symbol)->f | SolLL1ParserSymbolFlag_NULL)
#define solLL1ParserSymbol_set_nullable_computed(symbol) (symbol)->f = ((symbol)->f | SolLL1ParserSymbolFlag_Computed_NULL)
#define solLL1ParserSymbol_set_first_computed(symbol) (symbol)->f = ((symbol)->f | SolLL1ParserSymbolFlag_Computed_FIRST)
#define solLL1ParserSymbol_set_follow_computed(symbol) (symbol)->f = ((symbol)->f | SolLL1ParserSymbolFlag_Computed_FOLLOW)

#define solLL1Parser_table_set_hash_func1(p, f) solHash_set_hash_func1(solLL1Parser_table(p), f)
#define solLL1Parser_table_set_hash_func2(p, f) solHash_set_hash_func2(solLL1Parser_table(p), f)

#endif
