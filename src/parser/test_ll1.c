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

void inspect_product(SolLL1ParserProduct *f)
{
    if (f == NULL || solLL1ParserProduct_len(f) == 0) return;
    SolLL1ParserProductNode *n = solLL1ParserProduct_left(f);
    SolLL1ParserSymbol *s = solLL1ParserProductNode_symbol(n);
    int v = *(int*)(solLL1ParserSymbol_symbol(s));
    //printf("%d\t->\t", v);
    printf("%s.%d\t=>\t", cc[v-1], v);
    while ((n = solListNode_next(n))) {
        SolLL1ParserSymbol *s = solLL1ParserProductNode_symbol(n);
        int v = *(int*)(solLL1ParserSymbol_symbol(s));
        //printf(" %d", v);
        printf(" %s.%d ", cc[v-1], v);
    }
    printf("\n");
}

int print_entry_symbol(SolRBTree *t, SolRBTreeNode *n, void *v)
{
    if (solRBTree_node_is_nil(t, n)) return 0;
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

int print_symbol(SolLL1ParserSymbol *s)
{
    int d = *(int*)(solLL1ParserSymbol_symbol(s));
    if (d > Limit) {
        printf("entry data is not right!");
        return 1;
    }
    printf("%s ", cc[d-1]);
    return 0;
}

int print_entry(SolRBTree *t, SolRBTreeNode *n, void *v)
{
    if (solRBTree_node_is_nil(t, n)) return 0;
    SolLL1ParserEntry *e= solRBTreeNode_val(n);
    SolLL1ParserSymbol *s= solLL1ParserEntry_symbol(e);
    int d= *(int*)(solLL1ParserSymbol_symbol(s));
    if (d > Limit) {
        printf("entry data is not right!");
        return 1;
    }
    printf("Entry of <<%s>> , product: ", cc[d-1]);
    SolLL1ParserProduct *p = solLL1ParserEntry_product(e);
    if (solLL1ParserProduct_len(p)) {
        inspect_product(p);
    }
    return 0;
}

int inspect_rbtree(SolRBTree *t, solRBTree_f_ptr_act f)
{
    if (t == NULL || solRBTree_count(t) == 0) {
        printf("<<<None>>\n");
    } else {
        solRBTree_travelsal_inorder(t, solRBTree_root(t), f, NULL);
    }
    printf("\n");
    return 0;
}

SolLL1ParserSymbol* read_str(void *g)
{
    SolListNode *n = solListNode_next((SolListNode*)g);
    if (n) {
        return solListNode_val(n);
    }
    return NULL;
}

int _output(void *x, SolLL1ParserProduct *p, SolLL1ParserSymbol *s)
{
    if (p) {
        printf("output product\n");
    } else if (s) {
        printf("output symbol\n");
    } else {
        printf("output nothing\n");
    }
    return 0;
}

#define NONTERMINAL(S) SolLL1ParserSymbol *s##S = solLL1Parser_nonterminal(p, &c[S-1])
#define TERMINAL(S) SolLL1ParserSymbol *s##S = solLL1Parser_terminal(p, &c[S-1])
#define NULLABLE(S) SolLL1ParserSymbol *s##S = solLL1Parser_null(p, &c[S-1])
#define END_SYMBOL(S) SolLL1ParserSymbol *s##S = solLL1Parser_symbol_end(p, &c[S-1])

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
    END_SYMBOL(end);
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
    
    inspect_product(f1);
    inspect_product(f2);
    inspect_product(f3);
    inspect_product(f4);
    inspect_product(f5);
    inspect_product(f6);
    inspect_product(f7);
    inspect_product(f8);
    inspect_product(f9);
    inspect_product(f10);
    inspect_product(f11);
    inspect_product(f12);
    /*
    printf("compute nullable of S:\t%d\n", solLL1Parser_symbol_compute_nullable(p, sS));
    printf("compute nullable of E:\t%d\n", solLL1Parser_symbol_compute_nullable(p, sE));
    printf("compute nullable of E':\t%d\n", solLL1Parser_symbol_compute_nullable(p, sE1));
    printf("compute nullable of T:\t%d\n", solLL1Parser_symbol_compute_nullable(p, sT));
    printf("compute nullable of T':\t%d\n", solLL1Parser_symbol_compute_nullable(p, sT1));
    printf("compute nullable of F:\t%d\n", solLL1Parser_symbol_compute_nullable(p, sF));

    printf("compute first of S[%d]: ", solLL1Parser_symbol_compute_first(p, sS));
    if (inspect_rbtree(solLL1ParserSymbol_first(sS), &print_entry_symbol))
        printf("empty first\n");
    printf("compute first of E[%d]: ", solLL1Parser_symbol_compute_first(p, sE));
    if (inspect_rbtree(solLL1ParserSymbol_first(sE), &print_entry_symbol))
        printf("empty first\n");
    printf("compute first of E' [%d]: ", solLL1Parser_symbol_compute_first(p, sE1));
    if (inspect_rbtree(solLL1ParserSymbol_first(sE1), &print_entry_symbol))
        printf("empty first\n");
    printf("compute first of T [%d]: ", solLL1Parser_symbol_compute_first(p, sT));
    if (inspect_rbtree(solLL1ParserSymbol_first(sT), &print_entry_symbol))
        printf("empty first\n");
    printf("compute first of T' [%d]: ", solLL1Parser_symbol_compute_first(p, sT1));
    if (inspect_rbtree(solLL1ParserSymbol_first(sT1), &print_entry_symbol))
        printf("empty first\n");
    printf("compute first of F [%d]: ", solLL1Parser_symbol_compute_first(p, sF));
    if (inspect_rbtree(solLL1ParserSymbol_first(sF), &print_entry_symbol))
        printf("empty first\n");

    printf("compute follow of S[%d]: ", solLL1Parser_symbol_compute_follow(p, sS));
    if (inspect_rbtree(solLL1ParserSymbol_follow(sS), &print_entry_symbol))
        printf("empty follow\n");
    printf("compute follow of E[%d]: ", solLL1Parser_symbol_compute_follow(p, sE));
    if (inspect_rbtree(solLL1ParserSymbol_follow(sE), &print_entry_symbol))
        printf("empty follow\n");
    printf("compute follow of E'[%d]: ", solLL1Parser_symbol_compute_follow(p, sE1));
    if (inspect_rbtree(solLL1ParserSymbol_follow(sE1), &print_entry_symbol))
        printf("empty follow\n");
    printf("compute follow of T[%d]: ", solLL1Parser_symbol_compute_follow(p, sT));
    if (inspect_rbtree(solLL1ParserSymbol_follow(sT), &print_entry_symbol))
        printf("empty follow\n");
    printf("compute follow of T'[%d]: ", solLL1Parser_symbol_compute_follow(p, sT1));
    if (inspect_rbtree(solLL1ParserSymbol_follow(sT1), &print_entry_symbol))
        printf("empty follow\n");
    printf("compute follow of F[%d]: ", solLL1Parser_symbol_compute_follow(p, sF));
    if (inspect_rbtree(solLL1ParserSymbol_follow(sF), &print_entry_symbol))
        printf("empty follow\n");
    */
    printf("Generate table return %d\n\n", solLL1Parser_generate_table(p));

    printf("S first:\n");
    inspect_rbtree(solLL1ParserSymbol_first(sS), &print_entry);
    printf("S follow (nullable [%d]):\n", solLL1ParserSymbol_is_nullable(sS));
    inspect_rbtree(solLL1ParserSymbol_follow(sS), &print_entry);

    printf("E first:\n");
    inspect_rbtree(solLL1ParserSymbol_first(sE), &print_entry);
    printf("E follow (nullable [%d]):\n", solLL1ParserSymbol_is_nullable(sE));
    inspect_rbtree(solLL1ParserSymbol_follow(sE), &print_entry);

    printf("E' first:\n");
    inspect_rbtree(solLL1ParserSymbol_first(sE1), &print_entry);
    printf("E' follow (nullable [%d]):\n", solLL1ParserSymbol_is_nullable(sE1));
    inspect_rbtree(solLL1ParserSymbol_follow(sE1), &print_entry);

    printf("T first:\n");
    inspect_rbtree(solLL1ParserSymbol_first(sT), &print_entry);
    printf("T follow (nullable [%d]):\n", solLL1ParserSymbol_is_nullable(sT));
    inspect_rbtree(solLL1ParserSymbol_follow(sT), &print_entry);

    printf("T' first:\n");
    inspect_rbtree(solLL1ParserSymbol_first(sT1), &print_entry);
    printf("T' follow (nullable [%d]):\n", solLL1ParserSymbol_is_nullable(sT1));
    inspect_rbtree(solLL1ParserSymbol_follow(sT1), &print_entry);

    printf("F first:\n");
    inspect_rbtree(solLL1ParserSymbol_first(sF), &print_entry);
    printf("F follow (nullable [%d]):\n", solLL1ParserSymbol_is_nullable(sF));
    inspect_rbtree(solLL1ParserSymbol_follow(sF), &print_entry);

    SolList *l = solList_new();
    // int + int * (int + int)$
    solList_add(l, snum);
    solList_add(l, splus);
    solList_add(l, snum);
    solList_add(l, smul);
    solList_add(l, slc);
    solList_add(l, snum);
    solList_add(l, splus);
    solList_add(l, snum);
    solList_add(l, src);
    solList_add(l, send);
    solLL1Parser_set_read_symbol_func(p, &read_str);
    solLL1Parser_set_output_func(p, &_output);
    solLL1Parser_set_start_symbol(p, sS);
    solLL1Parser_parse(p, solList_head(l), NULL);

    solList_free(l);
    solLL1Parser_free(p);

    return 0;
}
