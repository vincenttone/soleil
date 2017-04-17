#include <string.h>
#include "sol_utils.h"

size_t sol_hash_func1(void *d)
{
	void *v;
	if (((SolVal*)d)->t == SolValTypeInt) {
		v = &((SolVal*)d)->v.i;
	} else if (((SolVal*)d)->t == SolValTypeChar) {
		v = &((SolVal*)d)->v.c;
	} else if (((SolVal*)d)->t == SolValTypeStr) {
		v = ((SolVal*)d)->v.s;
	}
	return MurmurHash2(v, ((SolVal*)d)->l, 0);
}

size_t sol_hash_func2(void *d)
{
	void *v;
	if (((SolVal*)d)->t == SolValTypeInt) {
		v = &((SolVal*)d)->v.i;
	} else if (((SolVal*)d)->t == SolValTypeChar) {
		v = &((SolVal*)d)->v.c;
	} else if (((SolVal*)d)->t == SolValTypeStr) {
		v = ((SolVal*)d)->v.s;
	}
	return (size_t)fnv_32_buf(v, ((SolVal*)d)->l, FNV1_32_INIT);
}

int solVal_equal(SolVal *v1, SolVal *v2)
{
	if (v1->t == v2->t) {
		if (v1->l == v2->l) {
			if (v1->t == SolValTypeInt || v1->t == SolValTypeChar) {
				if (v1->v.i == v2->v.i) {
					return 0;
				}
			} else if (v1->t == SolValTypeStr) {
				return strcmp(v1->v.s, v2->v.s);
			}
		}
	}
	return 1;
}
