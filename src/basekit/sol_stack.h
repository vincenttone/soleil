#ifndef _SOL_STACK_H_
#define _SOL_STACK_H_ 1

#include <stddef.h>
#include "sol_common.h"
#include "sol_list.h"

typedef SolList SolStack;

SolStack* solStack_new();
#define solStack_free(s) solList_free(s)
#define solStack_push(s, d) solList_add(s, d, _SolListDirFwd)
void* solStack_pop(SolStack*);

#endif
