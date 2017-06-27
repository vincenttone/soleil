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
    if (solLL1Parser_stack(p) == NULL
        || solLL1Parser_table(p) == NULL
        || solLL1Parser_form_list(p) == NULL
        ) {
        solLL1Parser_free(p);
        return NULL;
    }
    solList_set_val_free_func(solLL1Parser_form_list(p), &_solLL1ParserForm_free);
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
    sol_free(p);
}

int solLL1Parser_add_form(SolLL1Parser *p, SolLL1ParserForm *f)
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

SolLL1ParserForm* solLL1ParserForm_new(SolLL1ParserSign *s)
{
    if (s == NULL) {
        return NULL;
    }
    SolLL1ParserForm *f = sol_alloc(sizeof(SolLL1ParserForm));
    if (f == NULL) {
        return NULL;
    }
    solLL1ParserForm_set_sign(f, s);
    solLL1ParserForm_set_next(f, NULL);
    return f;
}

int solLL1ParserForm_add_sign(SolLL1ParserForm *f, SolLL1ParserSign *s)
{
    if (f == NULL) {
        return -1;
    }
    SolLL1ParserForm *nf = solLL1ParserForm_new(s);
    if (nf == NULL) {
        return -2;
    }
    SolLL1ParserForm *lf;
    do {
        lf = f;
    } while ((f = solLL1ParserForm_next(f)));
    solLL1ParserForm_set_next(lf, nf);
    return 0;
}

int solLL1ParserForm_add_terminal(SolLL1ParserForm *f, void *s)
{
    SolLL1ParserSign *sign = solLL1ParserSign_new(s, SolLL1ParserSignType_Terminal);
    if (sign == NULL) {
        return -1;
    }
    if (solLL1ParserForm_add_sign(f, sign) == 0) {
        return 0;
    }
    return -2;
}

int solLL1ParserForm_add_nonterminal(SolLL1ParserForm *f, void *s)
{
    SolLL1ParserSign *sign = solLL1ParserSign_new(s, SolLL1ParserSignType_Nonterminal);
    if (sign == NULL) {
        return -1;
    }
    if (solLL1ParserForm_add_sign(f, sign) == 0) {
        return 0;
    }
    return -2;
}

void _solLL1ParserForm_free(void *f)
{
    solLL1ParserForm_free((SolLL1ParserForm*)f);
}

void solLL1ParserForm_free(SolLL1ParserForm *f)
{
    SolLL1ParserForm *n;
    while (f) {
        n = solLL1ParserForm_next(f);
        if (solLL1ParserForm_sign(f)) {
            solLL1ParserSign_free(solLL1ParserForm_sign(f));
        }
        sol_free(f);
        f = n;
    }
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
    if (s) {
        sol_free(s);
    }
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
