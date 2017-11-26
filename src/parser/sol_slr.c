#include "sol_slr.h"

int solSLRParser_generate_items(SolSLRParser *p)
{
    if (p == NULL || p->p == NULL) {
        return -1;
    }
    if (solList_len(p->p) == 0) {
        return -2;
    }
    SolLRProduct *c = solList_head(p->p);
    SolLRSymbol *s;
    SolLRSymbol *rs;
    SolLRSymbol *cts;
    size_t i = 0;
    size_t j;
    while (i < solList_len(p)) {
        c = (SolLRProduct*)(solListNode_next(c));
        s = (SolLRSymbol*)(solLRProduct_left(c));
        r = solLRProduct_right(c);
        j = 0;
        do {
            if (solLRSymbol_add_item(s, c, j) > 0) {
                return 1;  // add item of product failed
            }
            rs = solLRProduct_find_symbol(c, j);
            if (cts) { // compute goto
                solLRSymbol_add_goto(cts, c, j);
            }
            if (solLRSymbol_is_nonterminal(rs)) {
                if (solLRSymbol_add_nonterminal_items(s, rs) > 0) {
                    return 2; // add item of nonterminal failed
                }
                cts = NULL;
            } else if (solLRSymbol_is_terminal(rs)) {
                cts = rs;
            }
            r = solListNode_next(r);
        } while (++j < solLRProduct_len(c));
        cts = NULL;
    }
    return 0;
}
