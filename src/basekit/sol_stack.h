#ifndef _SOL_STACK_H_
#define _SOL_STACK_H_ 1

#include <stddef.h>
#include "sol_common.h"
#include "sol_dl_list.h"

typedef SolDlList SolStack;

SolStack* solStack_new();
#define solStack_free(s) solDlList_free(s)
#define solStack_push(s, d) solDlList_add(s, d, _SolDlListDirFwd)
void* solStack_pop(SolStack*);

#endif
