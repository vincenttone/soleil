#include <stdio.h>
#include "sol_lr.h"
#include "sol_slr.h"

enum _Symbol {
    _E = 1,
    _T,
    _F,
    _lc,
    _rc,
    _plus,
    _mul,
    _id,
    _limit,
};

#define solSLRParser_NONTERMINAL(v, p, x) SolLRSymbol *v = solSLRParser_nonterminal_new(p, &(x[_##v - 1]))
#define solSLRParser_TERMINAL(v, p, x)    SolLRSymbol *v = solSLRParser_terminal_new(p, &(x[_##v - 1]))

int cmp(void *v1, void *v2)
{
    if (*(int*)v1 > *(int*)v2) {
        return 1;
    } else if (*(int*)v1 < *(int*)v2) {
        return -1;
    }
    return 0;
}

void out_symbol_val(void *v, char *pre, char *after)
{
    char *s;
    if (v == NULL) {
        s = " -O- ";
    } else {
        char a[_limit - 1][3] = {"E", "T", "F", "(", ")", "+", "*", "id"};
        s = a[*(int*)v - 1];
    }
    printf("%s%s%s", pre, s, after);
}

void out_symbol(SolLRSymbol *s, SolSLRParser *p)
{
    if (s == p->s) {
        printf("Origin");
    } else {
        out_symbol_val(s->v, "", "");
    }
}

void out_product(SolLRProduct *product, SolSLRParser *p)
{
    printf("Product:\t");
    out_symbol(product->s, p);
    printf(" => ");
    size_t i;
    for (i = 0; i < product->len; i++) {
        out_symbol(*((SolLRSymbol**)(product->r + i)), p);
    }
    printf("\n");
}

void out_item(SolLRItem *item, SolSLRParser *p)
{
    SolLRProduct *product = item->p;
    printf("Item:\t");
    out_symbol(product->s, p);
    printf(" => ");
    size_t i;
    for (i = 0; i < product->len; i++) {
        if (i == item->pos) {
            printf(".");
        }
        out_symbol(*((SolLRSymbol**)(product->r + i)), p);
    }
    if (i == item->pos) {
        printf(".");
    }
    printf("\n");
}

int main()
{
    int symbols[] = {_E, _T, _F, _lc, _rc, _plus, _mul, _id};
    SolSLRParser *p = solSLRParser_new();
#ifdef __SOL_DEBUG__
    p->_f_debug_symbol = &out_symbol;
    p->_f_debug_product = &out_product;
    p->_f_debug_item = &out_item;
#endif
    solSLRParser_set_compare_symbol_val_func(p, &cmp);
    // symbols
    solSLRParser_NONTERMINAL(E, p, symbols);
    solSLRParser_NONTERMINAL(T, p, symbols);
    solSLRParser_NONTERMINAL(F, p, symbols);
    solSLRParser_TERMINAL(lc, p, symbols);
    solSLRParser_TERMINAL(rc, p, symbols);
    solSLRParser_TERMINAL(plus, p, symbols);
    solSLRParser_TERMINAL(mul, p, symbols);
    solSLRParser_TERMINAL(id, p, symbols);
    // productions
    SolLRProduct *product = solLRProduct_new(E, 3, E, plus, T); // E -> E + T
    out_product(product, p);
    printf("set begin product ret: %d\n", solSLRParser_set_begin_product(p, product));
    product = solLRProduct_new(E, 1, T);          // E -> T
    out_product(product, p);
    product = solLRProduct_new(T, 3, T, mul, F);  // T -> T * F
    out_product(product, p);
    product = solLRProduct_new(T, 1, F);          // T -> F
    out_product(product, p);
    product = solLRProduct_new(F, 3, lc, E, rc);  // F -> (E)
    out_product(product, p);
    product = solLRProduct_new(F, 1, id);         // F -> id
    out_product(product, p);
    printf("prepare return %d, collection count: %zu\n", solSLRParser_prepare(p), solList_len(p->lr->collections));
    //SolListNode *n = solList_head(p->lr->collections);

    solSLRParser_free(p);
    return 0;
}
