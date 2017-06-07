#include <stddef.h>
#include <assert.h>
#include "sol_list.h"

SolDlList* solDlList_new()
{
    SolDlList *l = sol_calloc(1, sizeof(SolDlList));
    if (l == NULL) {
        return NULL;
    }
    return l;
}

void solDlList_free(SolDlList *l)
{
    unsigned long len;
    SolDlListNode *c, *n;
    c = l->head;
    len = l->len;
    if (c != NULL) {
        while (len--) {
            n = c->next;
            solDlListNodeVal_free(l, c);
            if (c != NULL) {
                sol_free(c);
            }
            c = n;
        }
    }
    sol_free(l);
}

SolDlListNode *solDlList_add(SolDlList *l, void *v, enum _SolDlListDir d)
{
    SolDlListNode  *n = sol_alloc(sizeof(SolDlListNode));
    if (n == NULL) {
        return NULL;
    }
    n->val = v;
    if (l->len == 0) {
        l->head = n;
        l->tail = n;
        n->pre = NULL;
        n->next = NULL;
    } else {
        if (d == _SolDlListDirBak) {
            n->pre = NULL;
            n->next = l->head;
            l->head->pre = n;
            l->head = n;
        } else {
            n->pre = l->tail;
            n->next = NULL;
            l->tail->next = n;
            l->tail = n;
        }
    }
    l->len++;
    return n;
}

void solDlList_del_node(SolDlList *l, SolDlListNode *n)
{
    if (n->pre) {
        n->pre->next = n->next;
    } else {
        l->head = n->next;
    }
    if (n->next) {
        n->next->pre = n->pre;
    } else {
        l->tail = n->pre;
    }
    solDlListNodeVal_free(l, n);
    l->len--;
    sol_free(n);
}

int solDlList_merge(SolDlList *l1, SolDlList *l2)
{
    if (l1 == NULL || l2 == NULL) {
        return -1;
    }
    if (l1->head == NULL) {
        l1->head = l2->head;
        l1->tail = l2->tail;
        goto finish;
    }
    assert(solDlList_tail(l1) && "seems like list do not have tail");
    l1->tail->next = l2->head;
    if (l2->head) {
        l2->head->pre = l1->tail;
    }
    if (l2->tail) {
        l1->tail = l2->tail;
    }
finish:
    l1->len += l2->len;
    l2->head = NULL;
    l2->tail = NULL;
    l2->len = 0;
    solDlList_free(l2);
    return 0;
}

SolDlListIter* solDlListIter_new(SolDlList *l, enum _SolDlListDir d)
{
    if (l == NULL) {
        return NULL;
    }
    SolDlListIter *i = sol_alloc(sizeof(SolDlListIter));
    if (i == NULL) {
        return NULL;
    }
    i->l = l;
    i->dir = d;
    if (d == _SolDlListDirBak) {
        i->next = l->tail;
    } else {
        i->next = l->head;
    }
    return i;
}

void solDlListIter_free(SolDlListIter *i)
{
    if (i != NULL) {
        sol_free(i);
    }
}

void solDlListIter_rewind(SolDlListIter *i)
{
    if (i->dir == _SolDlListDirBak) {
        i->next = i->l->tail;
    } else {
        i->next = i->l->head;
    }
}

SolDlListNode* solDlListIter_next(SolDlListIter *i)
{
    SolDlListNode *cn = i->next;
    if (cn != NULL) {
        if (i->dir == _SolDlListDirBak) {
            i->next = cn->pre;
        } else {
            i->next = cn->next;
        }
    }
    return cn;
}
