#ifndef _SOL_COMMON_H_
#define _SOL_COMMON_H_ 1

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define SolNil NULL

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

#define solList_new() vListInit(NULL)
#define solList_free vListRelease
#define solList_len(l) l->len
#define solList_add_node(l, v) vListAddNode(l, v, vListDirectionForward)
#define solList_del_node vListDelNode
#define solListIter_new(l) vListGetIterator(l, vListDirectionForward)
#define solListIter_free vListReleaseIterator
#define solListIter_next vListNextNode
#define solListIter_rewind vListRewind

#include "sol_hash.h"
#include "sol_set.h"

#include "Hash_fnv.h"
#include "Hash_murmur.h"

size_t solCommon_hash_func_murmur(void*);
size_t solCommon_hash_func_fnv32(void*);
int solCommon_string_equals(void *, void*);
int solCommon_char_equals(void *, void*);

SolHash* solHash_new_and_init();
SolSet* solSet_new_and_init();
SolSet* solSet_for_char_new_and_init();

#endif
