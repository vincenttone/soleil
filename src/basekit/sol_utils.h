#ifndef _SOL_UTILS_H_
#define _SOL_UTILS_H_ 1

#include <stddef.h>
#include <string.h>

#include "sol_common.h"
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

size_t solUtil_hash_func_murmur(void*);
size_t solUtil_hash_func_fnv32(void*);
int solUtil_string_equals(void *, void*);

SolHash* solHash_new_and_init();
SolSet* solSet_new_and_init();
inline void solHash_set_funcs_for_string(SolHash *h);

#endif