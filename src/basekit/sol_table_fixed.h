#ifndef _SOL_TABLE_FIXED_
#define _SOL_TABLE_FIXED_ 1

#include "sol_common.h"

typedef struct _SolTableFixed {
	void **vals;
	size_t cs;
	size_t rs;
} SolTableFixed;

typedef struct _SolTableFixedIter {
	SolTableFixed *tbl;
	size_t cc;
	size_t rc;
	int flag;
} SolTableFixedIter;

SolTableFixed* solTableFixed_new(size_t, size_t);
void solTableFixed_free(SolTableFixed*);

int solTableFixed_put(SolTableFixed*, size_t, size_t, void*);
void* solTableFixed_get(SolTableFixed*, size_t, size_t);
SolTableFixedIter* solTableFixed_get_rows(SolTableFixed*, size_t);
SolTableFixedIter* solTableFixed_get_columns(SolTableFixed*, size_t);

SolTableFixedIter* solTableFixedIter_new(SolTableFixed*, size_t, size_t, int);
void solTableFixedIter_free(SolTableFixedIter*);
void* solTableFixedIter_current(SolTableFixedIter*);
int solTableFixedIter_next(SolTableFixedIter*);

#endif
