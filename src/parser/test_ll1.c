#include <stdio.h>
#include "sol_list.h"
#include "sol_ll1.h"

enum _Symbol {
    S = 1,
    E,
    Op,
    Lc,
    Rc,
    Plus,
    Mul,
    id,
    end,
    Limit,
};

enum _Symbol c[Limit-1] = {S, E, Op, Lc, Rc, Plus, Mul, id, end};
char cc[Limit-1] = {'S', 'E', 'O', '(', ')', '+', '*', 'i', '$'};

void inspect_product(SolLL1Parser *p, SolLL1ParserProduct *f)
{
    SolListNode *n = solList_head(f);
    SolLL1ParserSymbol *s = (SolLL1ParserSymbol*)(solListNode_val(n));
    int v = *(int*)(solLL1ParserSymbol_symbol(s));
    //printf("%d\t->\t", v);
    printf("%c.%d\t=>\t", cc[v-1], v);
    while ((n = solListNode_next(n))) {
        SolLL1ParserSymbol *s = (SolLL1ParserSymbol*)(solListNode_val(n));
        int v = *(int*)(solLL1ParserSymbol_symbol(s));
        //printf(" %d", v);
        printf(" %c.%d ", cc[v-1], v);
    }
    printf("\n");
}

int inspect_symbol_list(SolList *l)
{
    if (solList_len(l) == 0) {
        return 1;
    }
    SolListNode *n = solList_head(l);
    SolLL1ParserSymbol *s;
    int v;
    do {
        s= (SolLL1ParserSymbol*)(solListNode_val(n));
        v= *(int*)(solLL1ParserSymbol_symbol(s));
        printf("%c ", cc[v-1]);
    } while ((n = solListNode_next(n)));
    printf("\n");
    return 0;
}

int main()
{
    SolLL1Parser *p = solLL1Parser_new();
    SolLL1ParserSymbol *sS = solLL1Parser_nonterminal(p, &c);
    SolLL1ParserSymbol *sE = solLL1Parser_nonterminal(p, &c[1]);
    SolLL1ParserSymbol *sOp = solLL1Parser_nonterminal(p, &c[2]);
    SolLL1ParserSymbol *sLc = solLL1Parser_terminal(p, &c[3]);
    SolLL1ParserSymbol *sRc = solLL1Parser_terminal(p, &c[4]);
    SolLL1ParserSymbol *sPlus = solLL1Parser_terminal(p, &c[5]);
    SolLL1ParserSymbol *sMul = solLL1Parser_terminal(p, &c[6]);
    SolLL1ParserSymbol *sid = solLL1Parser_terminal(p, &c[7]);
    SolLL1ParserSymbol *send = solLL1Parser_terminal(p, &c[8]);
    
    SolLL1ParserProduct *f1 = solLL1ParserProduct_new();
    solLL1ParserProduct_add_symbol(f1, sS);
    solLL1ParserProduct_add_symbol(f1, sE);
    solLL1ParserProduct_add_symbol(f1, send);
    
    SolLL1ParserProduct *f2 = solLL1ParserProduct_new();
        solLL1ParserProduct_add_symbol(f2, sE);
    solLL1ParserProduct_add_symbol(f2, sid);

    SolLL1ParserProduct *f3 = solLL1ParserProduct_new();
    solLL1ParserProduct_add_symbol(f3, sE);
    solLL1ParserProduct_add_symbol(f3, sLc);
    solLL1ParserProduct_add_symbol(f3, sE);
    solLL1ParserProduct_add_symbol(f3, sOp);
    solLL1ParserProduct_add_symbol(f3, sE);
    solLL1ParserProduct_add_symbol(f3, sRc);

    SolLL1ParserProduct *f4 = solLL1ParserProduct_new();
    solLL1ParserProduct_add_symbol(f4, sOp);
    solLL1ParserProduct_add_symbol(f4, sPlus);

    SolLL1ParserProduct *f5 = solLL1ParserProduct_new();
    solLL1ParserProduct_add_symbol(f5, sOp);
    solLL1ParserProduct_add_symbol(f5, sMul);

    solLL1Parser_reg_product(p, f1);
    solLL1Parser_reg_product(p, f2);
    solLL1Parser_reg_product(p, f3);
    solLL1Parser_reg_product(p, f4);
    solLL1Parser_reg_product(p, f5);
    
    inspect_product(p, f1);
    inspect_product(p, f2);
    inspect_product(p, f3);
    inspect_product(p, f4);
    inspect_product(p, f5);

    printf("compute nullable of S:\t%d\n", solLL1Parser_symbol_compute_nullable(p, sS));
    printf("compute nullable of E:\t%d\n", solLL1Parser_symbol_compute_nullable(p, sE));
    printf("compute nullable of Op:\t%d\n", solLL1Parser_symbol_compute_nullable(p, sOp));

    printf("compute first of S[%d]:\t", solLL1Parser_symbol_compute_first(p, sS));
    if (inspect_symbol_list(solLL1ParserSymbol_first(sS)))
        printf("empty first\n");
    printf("compute first of E[%d]:\t", solLL1Parser_symbol_compute_first(p, sE));
    if (inspect_symbol_list(solLL1ParserSymbol_first(sE)))
        printf("empty first\n");
    printf("compute first of Op[%d]:\t", solLL1Parser_symbol_compute_first(p, sOp));
    if (inspect_symbol_list(solLL1ParserSymbol_first(sOp)))
        printf("empty first\n");

    printf("compute follow of S[%d]:\t", solLL1Parser_symbol_compute_follow(p, sS));
    if (inspect_symbol_list(solLL1ParserSymbol_follow(sS)))
        printf("empty follow\n");
    printf("compute follow of E[%d]:\t", solLL1Parser_symbol_compute_follow(p, sE));
    if (inspect_symbol_list(solLL1ParserSymbol_follow(sE)))
        printf("empty follow\n");
    printf("compute follow of Op[%d]:\t", solLL1Parser_symbol_compute_follow(p, sOp));
    if (inspect_symbol_list(solLL1ParserSymbol_follow(sOp)))
        printf("empty follow\n");

    solLL1Parser_free(p);

    return 0;
}
