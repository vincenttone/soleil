#ifndef _SOL_UTILS_H_
#define _SOL_UTILS_H_

#include "sol_common.h"
#include "Hash_murmur.h"
#include "Hash_fnv.h"

size_t sol_hash_func1(void*, size_t);
size_t sol_hash_func2(void*, size_t);

size_t sol_i_hash_func1(void*);
size_t sol_i_hash_func2(void*);

size_t sol_c_hash_func1(void*);
size_t sol_c_hash_func2(void*);

size_t solVal_hash_func1(void*);
size_t solVal_hash_func2(void*);

int solVal_equal(SolVal*, SolVal*);

#endif
