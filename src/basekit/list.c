#include <stdlib.h>
#include "list.h"

vList *vListInit(vList *list)
{
	if (list == NULL) 
		return NULL;
	list->head = NULL;
	list->tail = NULL;
	list->len = 0;
	list->free = NULL;
	list->dup = NULL;
	return list;
}

void vListRelease(vList *list)
{
	unsigned long len;
	vListNode *current, *next;

	current = list->head;
	len = list->len;
	if (current != NULL) {
		while (len--) {
			next = current->next;
			if (list->free) (*list->free)(current->val);
			if (current != NULL) free(current);
			current = next;
		}
		free(list);
	}
}

vList *vListAddNode(vList *list, void *val, enum VListDirection direction)
{
	vListNode  *node;
	node = malloc(sizeof(vListNode));
	if (node == NULL)
		return NULL;
	node->val = val;
	if (list->len == 0) {
		list->head = node;
		list->tail = node;
		node->pre = NULL;
		node->next = NULL;
	} else {
		if (direction == vListDirectionBackward) {
			node->pre = NULL;
			node->next = list->head;
			list->head->pre = node;
			list->head = node;
		} else {
			node->pre = list->tail;
			node->next = NULL;
			list->tail->next = node;
			list->tail = node;
		}
	}
	list->len++;
	return list;
}

void vListDelNode(vList *list, vListNode *node)
{
	if (node->pre) {
		node->pre->next = node->next;
	} else {
		list->head = node->next;
	}
	if (node->next) {
		node->next->pre = node->pre;
	} else {
		list->tail = node->pre;
	}
	if (list->free) {
		(*list->free)(node->val);
	}
	list->len--;
	free(node);
}

vListIter *vListGetIterator(vList *list, enum VListDirection direction)
{
	vListIter *iterator;
	iterator = malloc(sizeof(vListIter));
	if (iterator == NULL) {
		return NULL;
	}
	iterator->direction = direction;
	if (direction == vListDirectionBackward) {
		iterator->next = list->tail;
	} else {
		iterator->next = list->head;
	}
	return iterator;
}

void vListReleaseIterator(vListIter *iterator)
{
	free(iterator);
}

void vListRewind(vList *list, vListIter *iterator)
{
	if (iterator->direction == vListDirectionBackward) {
		iterator->next = list->tail;
	} else {
		iterator->next = list->head;
	}
}

vListNode *vListNextNode(vListIter *iterator)
{
	vListNode *current = iterator->next;
	if (current != NULL) {
		if (iterator->direction == vListDirectionBackward) {
			iterator->next = current->pre;
		} else {
			iterator->next = current->next;
		}
	}
	return current;
}
