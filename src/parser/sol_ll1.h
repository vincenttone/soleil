#ifndef _SOL_LL1_PARSER_H_
#define _SOL_LL1_PARSER_H_ 1

#include "sol_common.h"
#include "sol_list.h"
#include "sol_dl_list.h"
#include "sol_stack.h"
#include "sol_hash.h"
#include "sol_rbtree.h"

#define SolLL1ParserSymbolFlag_NULL 0x1
#define SolLL1ParserSymbolFlag_END 0x2
#define SolLL1ParserSymbolFlag_Terminal 0x4
#define SolLL1ParserSymbolFlag_Nonterminal 0x8
#define SolLL1ParserSymbolFlag_Computed_NULL 0x10
#define SolLL1ParserSymbolFlag_Computed_FIRST 0x20
#define SolLL1ParserSymbolFlag_Computed_FOLLOW 0x40

#define SolLL1ParserProductFlag_NULLABLE 0x1

#define SolLL1ParserProductNode SolDlListNode

typedef struct _SolLL1ParserSymbol {
    int f; // flag
    void *s; // symbol
    SolRBTree *first;
    SolRBTree *follow;
    //SolLL1ParserProduct *np; // nullable product
    SolList *l; // product list
} SolLL1ParserSymbol;

typedef struct _SolLL1ParserProduct {
    int f;
    SolLL1ParserSymbol *s; // symbol
    SolDlList *r; // rule
} SolLL1ParserProduct;

typedef struct _SolLL1Parser {
    SolStack *s;
    SolList *fl; // product list
    SolRBTree *ss; // symbol list
    SolLL1ParserSymbol* start;
    SolLL1ParserSymbol* end;
    SolLL1ParserSymbol* (*f_read)(void*);
    // output ANY, PRODUCT, SYMBOL, IGNORE
    int (*f_out)(void*, SolLL1ParserProduct*, SolLL1ParserSymbol*, SolLL1ParserSymbol*);
} SolLL1Parser;

typedef struct _SolLL1ParserEntry {
    SolLL1ParserSymbol *s;
    SolLL1ParserProduct *p;
} SolLL1ParserEntry;

SolLL1Parser* solLL1Parser_new();
void solLL1Parser_free(SolLL1Parser*);
int solLL1Parser_reg_symbol(SolLL1Parser*, SolLL1ParserSymbol*);
int solLL1Parser_parse(SolLL1Parser*, void*, void*);

SolLL1ParserSymbol* solLL1Parser_terminal(SolLL1Parser*, void*);
SolLL1ParserSymbol* solLL1Parser_nonterminal(SolLL1Parser*, void*);
SolLL1ParserSymbol* solLL1Parser_null(SolLL1Parser*, void*);
SolLL1ParserSymbol* solLL1Parser_symbol_end(SolLL1Parser*, void*);

int solLL1Parser_generate_table(SolLL1Parser*);
int solLL1Parser_symbol_compute_first(SolLL1Parser*, SolLL1ParserSymbol*);
int solLL1Parser_symbol_compute_follow(SolLL1Parser*, SolLL1ParserSymbol*);
int solLL1Parser_symbol_compute_nullable(SolLL1Parser*, SolLL1ParserSymbol*);

SolLL1ParserProduct* solLL1ParserProduct_new(SolLL1Parser*, SolLL1ParserSymbol*);
void solLL1ParserProduct_free(SolLL1ParserProduct*);
#define solLL1ParserProduct_left(p) (p)->s
#define solLL1ParserProduct_right(p) (p)->r
#define solLL1ParserProduct_right_first(p) solDlList_head(solLL1ParserProduct_right(p))
#define solLL1ParserProduct_right_last(p) solDlList_tail(solLL1ParserProduct_right(p))
#define solLL1ParserProduct_right_next(n) solDlListNode_next(n)
#define solLL1ParserProductNode_symbol(n) solDlListNode_val(n)
#define solLL1ParserProduct_len(p) solDlList_len(solLL1ParserProduct_right(p))
#define solLL1ParserProduct_set_nullable(p) ((p)->f = (p)->f | SolLL1ParserProductFlag_NULLABLE)
#define solLL1ParserProduct_is_nullable(p) ((p)->f & SolLL1ParserProductFlag_NULLABLE > 0)

int solLL1ParserProduct_add_symbol(SolLL1ParserProduct*, SolLL1ParserSymbol*);
void _solLL1ParserProduct_free(void*);

SolLL1ParserSymbol* solLL1ParserSymbol_new(void*, int);
void solLL1ParserSymbol_free(SolLL1ParserSymbol*);
void _solLL1ParserSymbol_free(void*);
int solLL1ParserSymbol_add_first(SolLL1ParserSymbol*, SolLL1ParserSymbol*, SolLL1ParserProduct*);
int solLL1ParserSymbol_add_first_entry(SolLL1ParserSymbol*, SolLL1ParserEntry*);
int solLL1ParserSymbol_add_follow(SolLL1ParserSymbol*, SolLL1ParserSymbol*, SolLL1ParserProduct*);

int solLL1ParserSymbol_dup_first(SolLL1ParserSymbol*, SolRBTree*, SolLL1ParserProduct*);
int solLL1ParserSymbol_dup_follow(SolLL1ParserSymbol*, SolRBTree*, SolLL1ParserProduct*);

SolLL1ParserEntry* solLL1ParserEntry_new(SolLL1ParserSymbol*, SolLL1ParserProduct*);
void solLL1ParserEntry_free(SolLL1ParserEntry*);

int _solLL1Parser_entry_compare(void*, void*, int);
int _solLL1Parser_symbol_compare(void*, void*, int);
void _solLL1ParserEntry_free(void*);
int _solLL1Parser_rbnode_compute_nullable(SolRBTree*, SolRBTreeNode*, void*);
int _solLL1Parser_rbnode_compute_first(SolRBTree*, SolRBTreeNode*, void*);
int _solLL1Parser_rbnode_compute_follow(SolRBTree*, SolRBTreeNode*, void*);
int _solLL1Parser_rbnode_check(SolRBTree*, SolRBTreeNode*, void*);

#define solLL1Parser_set_stack(p, stack) (p)->s = stack
#define solLL1Parser_set_product_list(p, l) (p)->fl = l
#define solLL1Parser_set_symbol_list(p, l) (p)->ss = l
#define solLL1Parser_set_read_symbol_func(p, f) (p)->f_read = f
#define solLL1Parser_set_output_func(p, f) (p)->f_out = f
#define solLL1Parser_set_start_symbol(p, s) (p)->start = s
#define solLL1Parser_set_end_symbol(p, s) (p)->end = s

#define solLL1Parser_stack(p) (p)->s
#define solLL1Parser_product_list(p) (p)->fl
#define solLL1Parser_symbol_list(p) (p)->ss
#define solLL1Parser_read_symbol_func(p) (p)->f_read
#define solLL1Parser_output_func(p) (p)->f_out
#define solLL1Parser_start_symbol(p) (p)->start
#define solLL1Parser_end_symbol(p) (p)->end

#define solLL1Parser_read_symbol(p, g) (*(p)->f_read)(g)
#define solLL1Parser_output(p, x, product, symbol, ignore) (*(p)->f_out)(x, product, symbol, ignore)

#define solLL1ParserSymbol_set_symbol(symbol, d) (symbol)->s = d
#define solLL1ParserSymbol_set_type(symbol, type) (symbol)->f = (symbol)->f | type
#define solLL1ParserSymbol_set_first(symbol, f) (symbol)->first = f
#define solLL1ParserSymbol_set_follow(symbol, f) (symbol)->follow = f
#define solLL1ParserSymbol_set_product_list(symbol, list) (symbol)->l = list

#define solLL1ParserSymbol_symbol(symbol) (symbol)->s
#define solLL1ParserSymbol_first(symbol) (symbol)->first
#define solLL1ParserSymbol_follow(symbol) (symbol)->follow
#define solLL1ParserSymbol_flag(symbol) (symbol)->f
#define solLL1ParserSymbol_product_list(symbol) (symbol)->l

#define solLL1ParserSymbol_is_nonterminal(symbol) ((symbol)->f & SolLL1ParserSymbolFlag_Nonterminal)
#define solLL1ParserSymbol_is_terminal(symbol) ((symbol)->f & SolLL1ParserSymbolFlag_Terminal)
#define solLL1ParserSymbol_is_nullable(symbol) ((symbol)->f & SolLL1ParserSymbolFlag_NULL)
#define solLL1ParserSymbol_is_end(symbol) ((symbol)->f & SolLL1ParserSymbolFlag_END)
#define solLL1ParserSymbol_is_nullable_computed(symbol) ((symbol)->f & SolLL1ParserSymbolFlag_Computed_NULL)
#define solLL1ParserSymbol_is_first_computed(symbol) ((symbol)->f & SolLL1ParserSymbolFlag_Computed_FIRST)
#define solLL1ParserSymbol_is_follow_computed(symbol) ((symbol)->f & SolLL1ParserSymbolFlag_Computed_FOLLOW)

#define solLL1ParserSymbol_is_NOT_nullable(symbol) (((symbol)->f & SolLL1ParserSymbolFlag_NULL) == 0)
#define solLL1ParserSymbol_is_NOT_nonterminal(symbol) (((symbol)->f & SolLL1ParserSymbolFlag_Nonterminal) == 0)

#define solLL1ParserSymbol_set_nullable(symbol) (symbol)->f = ((symbol)->f | SolLL1ParserSymbolFlag_NULL)
#define solLL1ParserSymbol_set_nullable_computed(symbol) (symbol)->f = ((symbol)->f | SolLL1ParserSymbolFlag_Computed_NULL)
#define solLL1ParserSymbol_set_first_computed(symbol) (symbol)->f = ((symbol)->f | SolLL1ParserSymbolFlag_Computed_FIRST)
#define solLL1ParserSymbol_set_follow_computed(symbol) (symbol)->f = ((symbol)->f | SolLL1ParserSymbolFlag_Computed_FOLLOW)
//#define solLL1ParserSymbol_set_nullable_product(symbol, p) (symbol)->np = p

#define solLL1ParserEntry_set_symbol(e, symbol) (e)->s = symbol
#define solLL1ParserEntry_set_product(e, product) (e)->p = product

#define solLL1ParserEntry_symbol(e) (e)->s
#define solLL1ParserEntry_product(e) (e)->p

#define solLL1Parser_table_set_hash_func1(p, f) solHash_set_hash_func1(solLL1Parser_table(p), f)
#define solLL1Parser_table_set_hash_func2(p, f) solHash_set_hash_func2(solLL1Parser_table(p), f)

#endif
