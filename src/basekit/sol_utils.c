#include <string.h>
#include "sol_utils.h"

size_t sol_hash_func1(void *d)
{
	void *v;
	if (solVal_is_type_((SolVal*)d, SolValTypeInt)) {
		v = &solVal_int_val((SolVal*)d);
	} else if (solVal_is_type_((SolVal*)d,  SolValTypeChar)) {
		v = &solVal_char_val((SolVal*)d);
	} else if (solVal_is_type_((SolVal*)d, SolValTypeStr)) {
		v = solVal_str_val((SolVal*)d);
	}
	return MurmurHash2(v, solVal_get_size((SolVal*)d), 0);
}

size_t sol_hash_func2(void *d)
{
	void *v;
	if (solVal_is_type_((SolVal*)d, SolValTypeInt)) {
		v = &solVal_int_val((SolVal*)d);
	} else if (solVal_is_type_((SolVal*)d,  SolValTypeChar)) {
		v = &solVal_char_val((SolVal*)d);
	} else if (solVal_is_type_((SolVal*)d, SolValTypeStr)) {
		v = solVal_str_val((SolVal*)d);
	}
	return (size_t)fnv_32_buf(v, solVal_get_size((SolVal*)d), FNV1_32_INIT);
}

int solVal_equal(SolVal *v1, SolVal *v2)
{
	if (solVal_same_type(v1, v2)) {
		if (solVal_is_type_(v1, SolValTypeInt)) {
			if (solVal_int_val(v1) == solVal_int_val(v2)) {
				return 0;
			}
		} else if (solVal_is_type_(v1, SolValTypeChar)) {
			if (solVal_char_val(v1) == solVal_char_val(v2)) {
				return 0;
			}
		} else if (solVal_is_type_(v1, SolValTypeStr)) {
			if (solVal_same_size(v1, v2)) {
				return strcmp(v1->v.s, v2->v.s);
			}
		}
	}
	return 1;
}
