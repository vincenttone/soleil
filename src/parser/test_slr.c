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
};

#define solSLRParser_NONTERMINAL(v, p, x) SolLRSymbol *v = solSLRParser_nonterminal_new(p, &(x[_##v - 1]))
#define solSLRParser_TERMINAL(v, p, x)    SolLRSymbol *v = solSLRParser_terminal_new(p, &(x[_##v - 1]))

int main()
{
    int symbols[] = {_E, _T, _F, _lc, _rc, _plus, _mul, _id};
    SolSLRParser *p = solSLRParser_new();
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
    product = solLRProduct_new(E, 1, T);          // E -> T
    product = solLRProduct_new(T, 3, T, mul, F);  // T -> T * F
    product = solLRProduct_new(T, 1, F);          // T -> F
    product = solLRProduct_new(F, 3, lc, E, rc);  // F -> (E)
    product = solLRProduct_new(F, 1, id);         // F -> id
    printf("set begin product ret: %d\n", solSLRParser_set_begin_product(p, product));
    printf("prepare return %d\n", solSLRParser_prepare(p));
    solSLRParser_free(p);
    return 0;
}
