#ifndef _SOL_LL1_PARSER_H_
#define _SOL_LL1_PARSER_H_ 1

#include "sol_common.h"
#include "sol_list.h"
#include "sol_stack.h"
#include "sol_hash.h"

typedef enum _SolLL1ParserSignType {
    SolLL1ParserSignType_Nonterminal = 1,
    SolLL1ParserSignType_Terminal,
} SolLL1ParserSignType;

typedef struct _SolLL1ParserSign {
    void *s;
    SolLL1ParserSignType t;
} SolLL1ParserSign;

typedef struct _SolLL1ParserForm {
    SolLL1ParserSign *s;
    struct _SolLL1ParserForm *n;
} SolLL1ParserForm;

typedef struct _SolLL1Parser {
    SolHash *t;
    SolStack *s;
    SolList *fl; // form list
} SolLL1Parser;

typedef struct _SolLL1ParserTableK {
    SolLL1ParserSign *s1;
    SolLL1ParserSign *s2;
} SolLL1ParserTableK;

SolLL1Parser* solLL1Parser_new();
void solLL1Parser_free(SolLL1Parser*);
int solLL1Parser_add_form(SolLL1Parser*, SolLL1ParserForm*);

SolLL1ParserForm* solLL1ParserForm_new(SolLL1ParserSign*);
int solLL1ParserForm_add_sign(SolLL1ParserForm*, SolLL1ParserSign*);
int solLL1ParserForm_add_nonterminal(SolLL1ParserForm*, void*);
int solLL1ParserForm_add_terminal(SolLL1ParserForm*, void*);
void solLL1ParserForm_free(SolLL1ParserForm*);

SolLL1ParserSign* solLL1ParserSign_new(void*, SolLL1ParserSignType);
void solLL1ParserSign_free(SolLL1ParserSign*);

int solLL1Parser_table_add_rule(SolLL1Parser*, SolLL1ParserSign*, SolLL1ParserSign*, SolLL1ParserForm*);

void _solLL1ParserForm_free(void*);

#define solLL1Parser_set_stack(p, stack) (p)->s = stack
#define solLL1Parser_set_table(p, table) (p)->t = table
#define solLL1Parser_set_form_list(p, l) (p)->fl = l

#define solLL1Parser_stack(p) (p)->s
#define solLL1Parser_table(p) (p)->t
#define solLL1Parser_form_list(p) (p)->fl

#define solLL1ParserForm_set_sign(f, sign) (f)->s = sign
#define solLL1ParserForm_set_next(f, next) (f)->n = next

#define solLL1ParserForm_sign(f) (f)->s
#define solLL1ParserForm_next(f) (f)->n

#define solLL1ParserSign_set_sign(sign, d) (sign)->s = d
#define solLL1ParserSign_set_type(sign, type) (sign)->t = type

#define solLL1ParserSign_sign(sign) (sign)->s
#define solLL1ParserSign_type(sign) (sign)->t

#define solLL1Parser_table_set_hash_func1(p, f) solHash_set_hash_func1(solLL1Parser_table(p), f)
#define solLL1Parser_table_set_hash_func2(p, f) solHash_set_hash_func2(solLL1Parser_table(p), f)

#define solLL1ParserSign_terminal_new(s) solLL1ParserSign_new(s, SolLL1ParserSignType_Terminal)
#define solLL1ParserSign_nonterminal_new(s) solLL1ParserSign_new(s, SolLL1ParserSignType_Nonterminal)

#endif
