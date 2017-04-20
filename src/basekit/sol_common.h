#ifndef _SOL_COMMON_H_
#define _SOL_COMMON_H_ 1

#include <stdlib.h>
#include <stddef.h>

#define SolNil NULL

#define sol_alloc malloc
#define sol_calloc calloc
#define sol_free free
#define sol_realloc realloc

typedef int (*sol_f_match_ptr)(void*, void*);
typedef void (*sol_f_free_ptr)(void*);

enum SolValType {
	SolValTypeInt = 1,
	SolValTypeChar = 2,
	SolValTypeStr = 3,
};

typedef struct _SolVal {
	size_t s; // size
	union _v {
		int i;
		char c;
		char *s;
	} v; // val
	enum SolValType t; // type
} SolVal;

#define solVal_is_type_(v, type) (v)->t == type
#define solVal_same_type(v1, v2) (v1)->t == (v2)->t
#define solVal_same_size(v1, v2) (v1)->s == (v2)->s
#define solVal_get_size(v) (v)->s

#define solVal_int(sv, val) (sv)->v.i = val; (sv)->s = sizeof(int); (sv)->t = SolValTypeInt
#define solVal_char(sv, val) (sv)->v.c = val; (sv)->s = sizeof(char); (sv)->t = SolValTypeChar
#define solVal_str(sv, val, s) (sv)->v.s = val; (sv)->s = s; (sv)->t = SolValTypeStr

#define solVal_int_val(sv) (sv)->v.i
#define solVal_char_val(sv) (sv)->v.c
#define solVal_str_val(sv) (sv)->v.s


#endif
