#include <stddef.h>
#include "list.h"

SolList* solList_new()
{
	SolList *l = sol_alloc(sizeof(SolList));
	l->head = NULL;
	l->tail = NULL;
	l->len = 0;
	l->free = NULL;
	l->dup = NULL;
	return l;
}

void solList_free(SolList *l)
{
	unsigned long len;
	SolListNode *current, *next;
	current = l->head;
	len = l->len;
	if (current != NULL) {
		while (len--) {
			next = current->next;
			if (l->free) {
				(*l->free)(current->val);
			}
			if (current != NULL) {
				sol_free(current);
			}
			current = next;
		}
	}
	sol_free(l);
}

SolList *solList_add(SolList *l, void *v, enum SolListDir d)
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
		if (d == SolListDirBak) {
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
	if (l->free) {
		(*l->free)(n->val);
	}
	l->len--;
	sol_free(n);
}

SolListIter* solListIter_new(SolList *l, enum SolListDir d)
{
	SolListIter *i = sol_alloc(sizeof(SolListIter));
	if (i == NULL) {
		return NULL;
	}
	i->dir = d;
	if (d == SolListDirBak) {
		i->next = l->tail;
	} else {
		i->next = l->head;
	}
	return i;
}

void solListIter_free(SolListIter *i)
{
	sol_free(i);
}

void solList_rewind(SolList *l, SolListIter *i)
{
	if (i->dir == SolListDirBak) {
		i->next = l->tail;
	} else {
		i->next = l->head;
	}
}

SolListNode* solListIter_next(SolListIter *i)
{
	SolListNode *cn = i->next;
	if (cn != NULL) {
		if (i->dir == SolListDirBak) {
			i->next = cn->pre;
		} else {
			i->next = cn->next;
		}
	}
	return cn;
}
