#ifndef _V_LIST_H_
#define _V_LIST_H_ 1

enum VListDirection {
	vListDirectionForward = 1,
	vListDirectionBackward = 2,
};

typedef struct vListNode {
	void *val;
	struct vListNode *pre;
	struct vListNode *next;
} vListNode;

typedef struct {
	vListNode *head;
	vListNode *tail;
	unsigned long len;
	void *(*dup)(void *ptr);
	void (*free)(void *ptr);
	int (*match)(void *ptr);
	void *(*matchAndUpdate)(void *ptr);
} vList;

typedef struct {
	struct vListNode *next;
	enum VListDirection direction;
} vListIter;

vList *vListInit(vList *list);
void vListRelease(vList *list);
vList *vListAddNode(vList *list, void *val, enum VListDirection direction);
void vListDelNode(vList *list, vListNode *node);

vListIter *vListGetIterator(vList *list, enum VListDirection direction);
void vListRealseIterator(vListIter *iterator);
void vListRewind(vList *list, vListIter *iterator);
vListNode *vListNextNode(vListIter *iterator);

#endif
