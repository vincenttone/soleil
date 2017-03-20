#ifndef _SOL_COMMON_H_
#define _SOL_COMMON_H_ 1

#include <stdlib.h>

#define sol_alloc malloc
#define sol_calloc calloc
#define sol_free free
#define sol_realloc realloc

#include "list.h"
#define SolList vList
#define SolListNode vListNode
#define SolListIter vListIter

#define sol_list_init vListInit
#define sol_list_release vListRelease
#define sol_list_add_node(l, v) vListAddNode(l, v, vListDirectionForward)
#define sol_list_del_node vListDelNode
#define sol_list_get_iterator(l) vListGetIterator(l, vListDirectionForward)
#define sol_list_release_iterator vListReleaseIterator
#define sol_list_next_node vListNextNode
#define sol_list_rewind vListRewind

#endif
