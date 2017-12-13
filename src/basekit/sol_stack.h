#ifndef _SOL_STACK_H_
#define _SOL_STACK_H_ 1

#include <stddef.h>
#include "sol_common.h"
#include "sol_dl_list.h"

typedef SolDlList SolStack;

#define solStack_new() solDlList_new()
#define solStack_free(s) solDlList_free(s)
#define solStack_empty(s) (solDlList_len(s) == 0)
#define solStack_size(s) solDlList_len(s)
void* solStack_pop(SolStack*);
int solStack_push(SolStack*, void*);

#endif
