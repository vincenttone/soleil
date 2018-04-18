#include "sol_table_fixed.h"

SolTableFixed* solTableFixed_new(size_t cs, size_t rs)
{
	SolTableFixed *tbl = sol_alloc(sizeof(SolTableFixed));
	if (tbl == NULL) {
		return NULL;
	}
	tbl->cs = cs;
	tbl->rs = rs;
	tbl->vals = sol_calloc(cs * rs, sizeof(void*));
	if (tbl->vals == NULL) {
		solTableFixed_free(tbl);
		return NULL;
	}
	return tbl;
}

void solTableFixed_free(SolTableFixed *tbl)
{
	if (tbl->vals) {
		sol_free(tbl->vals);
	}
	sol_free(tbl);
}

int solTableFixed_put(SolTableFixed *tbl, size_t cc, size_t rc, void *val)
{
	if (cc >= tbl->cs || rc >= tbl->rs) {
		return -1;
	}
	*(tbl->vals + (tbl->rs * cc) + rc) = val;
	return 0;
}
void* solTableFixed_get(SolTableFixed *tbl, size_t cc, size_t rc)
{
	if (cc >= tbl->cs || rc >= tbl->rs) {
		return NULL;
	}
	return *(tbl->vals + (tbl->rs * cc) + rc);
}

SolTableFixedIter* solTableFixed_get_rows(SolTableFixed *tbl, size_t cc)
{
	if (cc >= tbl->cs) {
		return NULL;
	}
	return solTableFixedIter_new(tbl, cc, 0, 2);
}

SolTableFixedIter* solTableFixed_get_columns(SolTableFixed *tbl, size_t rc)
{
	if (rc >= tbl->rs) {
		return NULL;
	}
	return solTableFixedIter_new(tbl, 0, rc, 1);
}

SolTableFixedIter* solTableFixedIter_new(SolTableFixed *tbl, size_t cc, size_t rc, int flag)
{
	if (((flag & 0x1) && rc > tbl->rs) || ((flag & 0x2) && cc > tbl->cs)) {
		return NULL;
	}
	SolTableFixedIter *iter = sol_alloc(sizeof(SolTableFixedIter));
	if (iter == NULL) {
		return NULL;
	}
	iter->tbl = tbl;
	iter->cc = cc;
	iter->rc = rc;
	iter->flag = flag;
	return iter;
}

void solTableFixedIter_free(SolTableFixedIter *iter)
{
	sol_free(iter);
}

void* solTableFixedIter_current(SolTableFixedIter *iter)
{
    if (iter == NULL) {
        return NULL;
    }
	if ((iter->cc >= iter->tbl->cs)
        || (iter->rc >= iter->tbl->rs)
    ) {
		return NULL;
	}
	return *(iter->tbl->vals + (iter->cc * iter->tbl->rs) + iter->rc);
}

int solTableFixedIter_next(SolTableFixedIter *iter)
{
	if (iter->flag & 0x1) {
		if (++(iter->cc) >= iter->tbl->cs) {
			return 1;
		}
	} else if (iter->flag & 0x2) {
		if (++(iter->rc) >= iter->tbl->rs) {
			return 2;
		}
	} else {
		return -1;
	}
	return 0;
}
