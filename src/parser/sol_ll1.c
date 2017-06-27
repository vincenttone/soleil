#include "sol_ll1.h"

SolLL1Parser* solLL1Parser_new()
{
    SolLL1Parser *p = sol_alloc(sizeof(SolLL1Parser));
    if (p == NULL) {
        return NULL;
    }
    solLL1Parser_set_stack(p, solStack_new());
    solLL1Parser_set_table(p, solHash_new());
    solLL1Parser_set_form_list(p, solList_new());
    solLL1Parser_set_sign_list(p, solList_new());
    if (solLL1Parser_stack(p) == NULL
        || solLL1Parser_table(p) == NULL
        || solLL1Parser_form_list(p) == NULL
        ) {
        solLL1Parser_free(p);
        return NULL;
    }
    solList_set_val_free_func(solLL1Parser_form_list(p), &_solLL1ParserForm_free);
    solList_set_val_free_func(solLL1Parser_sign_list(p), &sol_free);
    solHash_set_free_k_func(solLL1Parser_table(p), &sol_free);
    return p;
}

void solLL1Parser_free(SolLL1Parser *p)
{
    if (solLL1Parser_stack(p)) {
        solStack_free(solLL1Parser_stack(p));
    }
    if (solLL1Parser_table(p)) {
        solHash_free(solLL1Parser_table(p));
    }
    if (solLL1Parser_form_list(p)) {
        solList_free(solLL1Parser_form_list(p));
    }
    if (solLL1Parser_sign_list(p)) {
        solList_free(solLL1Parser_sign_list(p));
    }
    sol_free(p);
}

int solLL1Parser_reg_form(SolLL1Parser *p, SolLL1ParserForm *f)
{
    if (p == NULL
        || solLL1Parser_form_list(p) == NULL
        || f == NULL
        ) {
        return -1;
    }
    if (solList_add(solLL1Parser_form_list(p), f)) {
        return 0;
    }
    return 1;
}

int solLL1Parser_reg_sign(SolLL1Parser *p, SolLL1ParserSign *s)
{
    if (p == NULL
        || solLL1Parser_sign_list(p) == NULL
        || s == NULL
        ) {
        return -1;
    }
    if (solList_add(solLL1Parser_sign_list(p), s)) {
        return 0;
    }
    return 1;
}

SolLL1ParserSign* solLL1Parser_reg_terminal(SolLL1Parser *p, void *s)
{
    SolLL1ParserSign *sign = solLL1ParserSign_new(s, SolLL1ParserSignType_Terminal);
    if (sign == NULL) return NULL;
    if (solLL1Parser_reg_sign(p, sign) == 0) {
        return sign;
    }
    solLL1ParserSign_free(sign);
    return NULL;
}

SolLL1ParserSign* solLL1Parser_reg_nonterminal(SolLL1Parser *p, void *s)
{
    SolLL1ParserSign *sign = solLL1ParserSign_new(s, SolLL1ParserSignType_Nonterminal);
    if (sign == NULL) return NULL;
    if (solLL1Parser_reg_sign(p, sign) == 0) {
        return sign;
    }
    solLL1ParserSign_free(sign);
    return NULL;
}

void _solLL1ParserForm_free(void *f)
{
    solLL1ParserForm_free((SolLL1ParserForm*)(f));
}

int solLL1ParserForm_add_sign(SolLL1ParserForm *f, SolLL1ParserSign *s)
{
    if (f == NULL || s == NULL) {
        return -1;
    }
    if (solList_add(f, s)) {
        return 0;
    }
    return 1;
}

SolLL1ParserSign* solLL1ParserSign_new(void *s, SolLL1ParserSignType t)
{
    SolLL1ParserSign *sign = sol_alloc(sizeof(SolLL1ParserSign));
    if (sign == NULL) {
        return NULL;
    }
    solLL1ParserSign_set_sign(sign, s);
    solLL1ParserSign_set_type(sign, t);
    return sign;
}

void solLL1ParserSign_free(SolLL1ParserSign *s)
{
    sol_free(s);
}

int solLL1Parser_table_add_rule(SolLL1Parser *p, SolLL1ParserSign *s1,
                                SolLL1ParserSign *s2, SolLL1ParserForm *f)
{
    if (p == NULL || s1 == NULL || s2 == NULL || f == NULL) {
        return -1;
    }
    SolLL1ParserTableK *k = sol_alloc(sizeof(SolLL1ParserTableK));
    if (k == NULL) {
        return -2;
    }
    k->s1 = s1;
    k->s2 = s2;
    if (solHash_put(solLL1Parser_table(p), k, f) == 0) {
        return 0;
    }
    return 1;
}
