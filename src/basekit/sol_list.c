#include <stddef.h>
#include "sol_list.h"

SolList* solList_new()
{
	SolList *l = sol_calloc(1, sizeof(SolList));
	if (l == NULL) {
		return NULL;
	}
	return l;
}

void solList_free(SolList *l)
{
	unsigned long len;
	SolListNode *c, *n;
	c = l->head;
	len = l->len;
	if (c != NULL) {
		while (len--) {
			n = c->next;
			solListNodeVal_free(l, c);
			if (c != NULL) {
				sol_free(c);
			}
			c = n;
		}
	}
	sol_free(l);
}

SolList *solList_add(SolList *l, void *v, enum _SolListDir d)
{
	SolListNode  *n = sol_alloc(sizeof(SolListNode));
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
		if (d == _SolListDirBak) {
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
	return l;
}

void solList_del_node(SolList *l, SolListNode *n)
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
	solListNodeVal_free(l, n);
	l->len--;
	sol_free(n);
}

SolListIter* solListIter_new(SolList *l, enum _SolListDir d)
{
	if (l == NULL) {
		return NULL;
	}
	SolListIter *i = sol_alloc(sizeof(SolListIter));
	if (i == NULL) {
		return NULL;
	}
	i->l = l;
	i->dir = d;
	if (d == _SolListDirBak) {
		i->next = l->tail;
	} else {
		i->next = l->head;
	}
	return i;
}

void solListIter_free(SolListIter *i)
{
	if (i != NULL) {
		sol_free(i);
	}
}

void solListIter_rewind(SolListIter *i)
{
	if (i->dir == _SolListDirBak) {
		i->next = i->l->tail;
	} else {
		i->next = i->l->head;
	}
}

SolListNode* solListIter_next(SolListIter *i)
{
	SolListNode *cn = i->next;
	if (cn != NULL) {
		if (i->dir == _SolListDirBak) {
			i->next = cn->pre;
		} else {
			i->next = cn->next;
		}
	}
	return cn;
}
