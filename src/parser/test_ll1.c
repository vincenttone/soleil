#include <stdio.h>
#include "sol_list.h"
#include "sol_ll1.h"
#include "sol_rbtree.h"

enum _Symbol {
    S = 1,
    E,
    E1,
    T,
    T1,
    F,
    Op,
    lc,
    rc,
    plus,
    sub,
    mul,
    divi,
    id,
    num,
    end,
    empty,
    Limit,
};

enum _Symbol c[Limit-1] = {
    S,
    E,
    E1,
    T,
    T1,
    F,
    Op,
    lc,
    rc,
    plus,
    sub,
    mul,
    divi,
    id,
    num,
    end,
    empty,
};
char cc[Limit-1][4] = {
    "S", "E", "E'", "T", "T'", "F", "Op",
    "(", ")", "+", "-", "*", "/", "id", "num",
    "$", "e",
};

void inspect_product(SolLL1Parser *p, SolLL1ParserProduct *f)
{
    SolListNode *n = solList_head(f);
    SolLL1ParserSymbol *s = (SolLL1ParserSymbol*)(solListNode_val(n));
    int v = *(int*)(solLL1ParserSymbol_symbol(s));
    //printf("%d\t->\t", v);
    printf("%s.%d\t=>\t", cc[v-1], v);
    while ((n = solListNode_next(n))) {
        SolLL1ParserSymbol *s = (SolLL1ParserSymbol*)(solListNode_val(n));
        int v = *(int*)(solLL1ParserSymbol_symbol(s));
        //printf(" %d", v);
        printf(" %s.%d ", cc[v-1], v);
    }
    printf("\n");
}

int print_symbol(SolRBTree *t, SolRBTreeNode *n, void *v)
{
    SolLL1ParserEntry *e= solRBTreeNode_val(n);
    SolLL1ParserSymbol *s= solLL1ParserEntry_symbol(e);
    int d= *(int*)(solLL1ParserSymbol_symbol(s));
    if (d > Limit) {
        printf("entry data is not right!");
        return 1;
    }
    printf("%s ", cc[d-1]);
    return 0;
}

int inspect_symbol_list(SolRBTree *t)
{
    if (solRBTree_count(t) == 0) {
        printf("<<<None>>");
    } else {
        solRBTree_travelsal_inorder(t, solRBTree_root(t), &print_symbol, NULL);
    }
    printf("\n");
    return 0;
}

#define NONTERMINAL(S) SolLL1ParserSymbol *s##S = solLL1Parser_nonterminal(p, &c[S-1])
#define TERMINAL(S) SolLL1ParserSymbol *s##S = solLL1Parser_terminal(p, &c[S-1])
#define NULLABLE(S) SolLL1ParserSymbol *s##S = solLL1Parser_null(p, &c[S-1])

int main()
{
    SolLL1Parser *p = solLL1Parser_new();
    NONTERMINAL(S);
    NONTERMINAL(E);
    NONTERMINAL(E1);
    NONTERMINAL(T);
    NONTERMINAL(T1);
    NONTERMINAL(F);
    // NONTERMINAL(Op);
    TERMINAL(lc);
    TERMINAL(rc);
    TERMINAL(plus);
    TERMINAL(sub);
    TERMINAL(mul);
    TERMINAL(divi);
    TERMINAL(id);
    TERMINAL(num);
    TERMINAL(end);
    NULLABLE(empty);
    
    SolLL1ParserProduct *f1 = solLL1ParserProduct_new();
    solLL1ParserProduct_add_symbol(f1, sS);
    solLL1ParserProduct_add_symbol(f1, sE);
    solLL1ParserProduct_add_symbol(f1, send);
    
    SolLL1ParserProduct *f2 = solLL1ParserProduct_new();
    solLL1ParserProduct_add_symbol(f2, sE);
    solLL1ParserProduct_add_symbol(f2, sT);
    solLL1ParserProduct_add_symbol(f2, sE1);

    SolLL1ParserProduct *f3 = solLL1ParserProduct_new();
    solLL1ParserProduct_add_symbol(f3, sE1);
    solLL1ParserProduct_add_symbol(f3, splus);
    solLL1ParserProduct_add_symbol(f3, sT);
    solLL1ParserProduct_add_symbol(f3, sE1);

    SolLL1ParserProduct *f4 = solLL1ParserProduct_new();
    solLL1ParserProduct_add_symbol(f4, sE1);
    solLL1ParserProduct_add_symbol(f4, ssub);
    solLL1ParserProduct_add_symbol(f4, sT);
    solLL1ParserProduct_add_symbol(f4, sE1);

    SolLL1ParserProduct *f5 = solLL1ParserProduct_new();
    solLL1ParserProduct_add_symbol(f5, sE1);
    solLL1ParserProduct_add_symbol(f5, sempty);

    SolLL1ParserProduct *f6 = solLL1ParserProduct_new();
    solLL1ParserProduct_add_symbol(f6, sT);
    solLL1ParserProduct_add_symbol(f6, sF);
    solLL1ParserProduct_add_symbol(f6, sT1);

    SolLL1ParserProduct *f7 = solLL1ParserProduct_new();
    solLL1ParserProduct_add_symbol(f7, sT1);
    solLL1ParserProduct_add_symbol(f7, smul);
    solLL1ParserProduct_add_symbol(f7, sF);
    solLL1ParserProduct_add_symbol(f7, sT1);

    SolLL1ParserProduct *f8 = solLL1ParserProduct_new();
    solLL1ParserProduct_add_symbol(f8, sT1);
    solLL1ParserProduct_add_symbol(f8, sdivi);
    solLL1ParserProduct_add_symbol(f8, sF);
    solLL1ParserProduct_add_symbol(f8, sT1);

    SolLL1ParserProduct *f9 = solLL1ParserProduct_new();
    solLL1ParserProduct_add_symbol(f9, sT1);
    solLL1ParserProduct_add_symbol(f9, sempty);

    SolLL1ParserProduct *f10 = solLL1ParserProduct_new();
    solLL1ParserProduct_add_symbol(f10, sF);
    solLL1ParserProduct_add_symbol(f10, sid);

    SolLL1ParserProduct *f11 = solLL1ParserProduct_new();
    solLL1ParserProduct_add_symbol(f11, sF);
    solLL1ParserProduct_add_symbol(f11, snum);

    SolLL1ParserProduct *f12 = solLL1ParserProduct_new();
    solLL1ParserProduct_add_symbol(f12, sF);
    solLL1ParserProduct_add_symbol(f12, slc);
    solLL1ParserProduct_add_symbol(f12, sE);
    solLL1ParserProduct_add_symbol(f12, src);

    solLL1Parser_reg_product(p, f1);
    solLL1Parser_reg_product(p, f2);
    solLL1Parser_reg_product(p, f3);
    solLL1Parser_reg_product(p, f4);
    solLL1Parser_reg_product(p, f5);
    solLL1Parser_reg_product(p, f6);
    solLL1Parser_reg_product(p, f7);
    solLL1Parser_reg_product(p, f8);
    solLL1Parser_reg_product(p, f9);
    solLL1Parser_reg_product(p, f10);
    solLL1Parser_reg_product(p, f11);
    solLL1Parser_reg_product(p, f12);
    
    inspect_product(p, f1);
    inspect_product(p, f2);
    inspect_product(p, f3);
    inspect_product(p, f4);
    inspect_product(p, f5);
    inspect_product(p, f6);
    inspect_product(p, f7);
    inspect_product(p, f8);
    inspect_product(p, f9);
    inspect_product(p, f10);
    inspect_product(p, f11);
    inspect_product(p, f12);

    printf("compute nullable of S:\t%d\n", solLL1Parser_symbol_compute_nullable(p, sS));
    printf("compute nullable of E:\t%d\n", solLL1Parser_symbol_compute_nullable(p, sE));
    printf("compute nullable of E':\t%d\n", solLL1Parser_symbol_compute_nullable(p, sE1));
    printf("compute nullable of T:\t%d\n", solLL1Parser_symbol_compute_nullable(p, sT));
    printf("compute nullable of T':\t%d\n", solLL1Parser_symbol_compute_nullable(p, sT1));
    printf("compute nullable of F:\t%d\n", solLL1Parser_symbol_compute_nullable(p, sF));

    printf("compute first of S[%d]: ", solLL1Parser_symbol_compute_first(p, sS));
    if (inspect_symbol_list(solLL1ParserSymbol_first(sS)))
        printf("empty first\n");
    printf("compute first of E[%d]: ", solLL1Parser_symbol_compute_first(p, sE));
    if (inspect_symbol_list(solLL1ParserSymbol_first(sE)))
        printf("empty first\n");
    printf("compute first of E' [%d]: ", solLL1Parser_symbol_compute_first(p, sE1));
    if (inspect_symbol_list(solLL1ParserSymbol_first(sE1)))
        printf("empty first\n");
    printf("compute first of T [%d]: ", solLL1Parser_symbol_compute_first(p, sT));
    if (inspect_symbol_list(solLL1ParserSymbol_first(sT)))
        printf("empty first\n");
    printf("compute first of T' [%d]: ", solLL1Parser_symbol_compute_first(p, sT1));
    if (inspect_symbol_list(solLL1ParserSymbol_first(sT1)))
        printf("empty first\n");
    printf("compute first of F [%d]: ", solLL1Parser_symbol_compute_first(p, sF));
    if (inspect_symbol_list(solLL1ParserSymbol_first(sF)))
        printf("empty first\n");

    printf("compute follow of S[%d]: ", solLL1Parser_symbol_compute_follow(p, sS));
    if (inspect_symbol_list(solLL1ParserSymbol_follow(sS)))
        printf("empty follow\n");
    printf("compute follow of E[%d]: ", solLL1Parser_symbol_compute_follow(p, sE));
    if (inspect_symbol_list(solLL1ParserSymbol_follow(sE)))
        printf("empty follow\n");
    printf("compute follow of E'[%d]: ", solLL1Parser_symbol_compute_follow(p, sE1));
    if (inspect_symbol_list(solLL1ParserSymbol_follow(sE1)))
        printf("empty follow\n");
    printf("compute follow of T[%d]: ", solLL1Parser_symbol_compute_follow(p, sT));
    if (inspect_symbol_list(solLL1ParserSymbol_follow(sT)))
        printf("empty follow\n");
    printf("compute follow of T'[%d]: ", solLL1Parser_symbol_compute_follow(p, sT1));
    if (inspect_symbol_list(solLL1ParserSymbol_follow(sT1)))
        printf("empty follow\n");
    printf("compute follow of F[%d]: ", solLL1Parser_symbol_compute_follow(p, sF));
    if (inspect_symbol_list(solLL1ParserSymbol_follow(sF)))
        printf("empty follow\n");

    solLL1Parser_free(p);

    return 0;
}
