#include <stdio.h>
#include "sol_lr.h"
#include "sol_slr.h"
#include "sol_rbtree_iter.h"

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

void out_symbol(SolLRSymbol *s, SolLRParser *p)
{
    if (s == p->origin) {
        printf("Origin");
    } else if (s == p->empty) {
        printf("Empty");
    } else if (s == p->end) {
        printf("$");
    } else {
        out_symbol_val(s->v, "", "");
    }
}

void out_product(SolLRProduct *product, SolLRParser *p)
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

int _travelsal_fileds(void *f, SolRBTuple *t, size_t level, void *d)
{
    int i;
    printf("|-");
    for (i = 0; i < level; i++) {
        printf("\t");
    }
    int flag = ((struct _SolSLRTableField*)f)->flag;
    if (flag & SolLRTableFieldFlag_TYPE_STATE) {
        SolLRItemCol *c = ((struct _SolSLRTableField*)f)->t;
        printf("<%zu>\t", c->state);
        // printf("<%zu>\t[%d]\t", c->state, ((struct _SolSLRTableField*)f)->flag);
    } else if (flag & SolLRTableFieldFlag_TYPE_SYMBOL) {
        SolLRSymbol *s = ((struct _SolSLRTableField*)f)->t;
        printf("[");
        out_symbol(s, (SolLRParser*)((SolSLRParser*)(t->ex))->lr);
        printf("]\t");
        //printf("]\t[%d]\t", ((struct _SolSLRTableField*)f)->flag);
    }
    if (flag & SolLRTableFieldFlag_ACTION_ACCEPT) {
        printf("ACCEPT");
    } else if (flag & SolLRTableFieldFlag_ACTION_REDUCE) {
        printf("REDUCE");
    } else if (flag & SolLRTableFieldFlag_ACTION_GOTO) {
        printf("GOTO");
    } else if (flag & SolLRTableFieldFlag_ACTION_SHIFT) {
        printf("SHIFT");
    }
    printf("\n");
    return 0;
}

int _travelsal_lr_fileds(void *f, SolRBTuple *t, size_t level, void *d)
{
    int i;
    printf("|-");
    for (i = 0; i < level; i++) {
        printf("\t");
    }
    int flag = ((SolLRTableField*)f)->flag;
    if (flag & SolLRTableFieldFlag_TYPE_STATE) {
        SolLRItemCol *c = ((SolLRTableField*)f)->target;
        printf(" <%zu>\t", c->state);
        // printf("<%zu>\t[%d]\t", c->state, ((struct _SolSLRTableField*)f)->flag);
    } else if (flag & SolLRTableFieldFlag_TYPE_SYMBOL) {
        SolLRSymbol *s = ((SolLRTableField*)f)->target;
        printf("[");
        out_symbol(s, (SolLRParser*)(t->ex));
        printf("]\t");
        //printf("]\t[%d]\t", ((struct _SolSLRTableField*)f)->flag);
    }
    if (flag & SolLRTableFieldFlag_ACTION_ACCEPT) {
        printf("ACCEPT");
    } else if (flag & SolLRTableFieldFlag_ACTION_REDUCE) {
        printf("REDUCE");
    } else if (flag & SolLRTableFieldFlag_ACTION_GOTO) {
        printf("GOTO");
    } else if (flag & SolLRTableFieldFlag_ACTION_SHIFT) {
        printf("SHIFT");
    }
    printf("\n");
    return 0;
}

void out_item(SolLRItem *item, SolLRParser *p)
{
    SolLRProduct *product = item->p;
    //printf("Item:\t");
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
    if (item->flag & 0x1) {
        printf("\t[FINAL]");
    }
    if (item->flag & 0x4) {
        printf("\t[FKNL]");
    }
    if (item->flag & 0x8) {
        printf("\t[FNKNL]");
    }
    printf("\t[%zu:%zu]", item->pos, item->p->len);
    if (item->cols && solList_len(item->cols)) {
        printf("\t[Cc:%zu]", solList_len(item->cols));
    }
    printf("\n");
}

int _out_rbtree_item(SolRBTree *tree, SolRBTreeNode *node, void *d)
{
    printf("\t");
    out_item((SolLRItem*)(solRBTreeNode_val(node)), (SolLRParser*)d);
    return 0;
}

void out_item_collections(SolLRItemCol *col, SolLRParser *p)
{
    if (col->state) {
        printf("Collection of state %zu, symbol: ", col->state);
    } else {
        printf("Collection of symbol: ");
    }
    if (col->sym) {
        out_symbol(col->sym, p);
    } else {
        printf("NULL");
    }
    if (col->items && solRBTree_count(col->items)) {
        printf("\n - Item(s):\n");
        solRBTree_travelsal_inorder(col->items, solRBTree_root(col->items), &_out_rbtree_item, p);
    } else {
        printf("\n - None item\n");
    }
}

int main()
{
    int symbols[] = {_E, _T, _F, _lc, _rc, _plus, _mul, _id};
    SolSLRParser *p = solSLRParser_new();
#ifdef __SOL_DEBUG__
    p->lr->f_debug_symbol = &out_symbol;
    p->lr->f_debug_product = &out_product;
    p->lr->f_debug_item = &out_item;
    p->lr->f_debug_item_col = &out_item_collections;
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
    out_product(product, p->lr);
    printf("set begin product ret: %d\n", solSLRParser_set_begin_product(p, product));
    product = solLRProduct_new(E, 1, T);          // E -> T
    out_product(product, p->lr);
    product = solLRProduct_new(T, 3, T, mul, F);  // T -> T * F
    out_product(product, p->lr);
    product = solLRProduct_new(T, 1, F);          // T -> F
    out_product(product, p->lr);
    product = solLRProduct_new(F, 3, lc, E, rc);  // F -> (E)
    out_product(product, p->lr);
    product = solLRProduct_new(F, 1, id);         // F -> id
    out_product(product, p->lr);
    printf("prepare return %d, collection count: %zu\n", solSLRParser_prepare(p), solList_len(p->lr->collections));
    p->lr->col_rel->f_travelsal_act = &_travelsal_lr_fileds;
    solRBTuple_travelsal(p->lr->col_rel, NULL);
    SolRBTreeIter *iter = solRBTreeIter_new(p->lr->col_rel->n, solRBTree_root(p->lr->col_rel->n), SolRBTreeIterTT_inorder);
    SolRBTupleRecord *record;
    SolLRTableField *field;
    do {
        record = solRBTreeIter_current_val(iter);
        field = record->v;
        if (field->flag & SolLRTableFieldFlag_COL_REPEATABLE) {
            printf("*******REPEATABLE********\n");
        }
        out_item_collections((SolLRItemCol*)(field->target), p->lr);
    } while (solRBTreeIter_next(iter));
    solRBTreeIter_free(iter);
    /*
    SolLRItemCol *col;
    SolListNode *n = solList_head(p->lr->collections);
    do {
        col = solListNode_val(n);
        out_item_collections(col, p->lr);
    } while ((n = solListNode_next(n)));
    p->table->f_travelsal_act = &_travelsal_fileds;
    solRBTuple_travelsal(p->table, NULL);
    */

    solSLRParser_free(p);
    return 0;
}
