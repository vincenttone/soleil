#ifndef _SOL_COMMON_H_
#define _SOL_COMMON_H_ 1

#include <stdlib.h>
#include <stddef.h>

#define SolNil NULL

#define sol_alloc malloc
#define sol_calloc calloc
#define sol_free free
#define sol_realloc realloc

enum SolValType {
	SolValTypeInt = 1,
	SolValTypeChar = 2,
	SolValTypeStr = 3,
};

typedef struct _SolVal {
	size_t l; // len
	union _v {
		int i;
		char c;
		char *s;
	} v;
	enum SolValType t; // type
} SolVal;

#endif
