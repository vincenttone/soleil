#ifndef _SOL_STACK_H_
#define _SOL_STACK_H_ 1

#include <stddef.h>
#include "sol_common.h"
#include "list.h"

typedef vList SolStack;

SolStack* solStack_new();
#define solStack_free(s) vListRelease(s)
#define solStack_push(s, d) vListAddNode(s, d, vListDirectionForward)
void* solStack_pop(SolStack*);

#endif
