#include "sol_utils.h"

size_t solUtil_hash_func_murmur(void *key)
{
	int len = strlen((char *)key);
	return MurmurHash2(key, len, 0);
}

size_t solUtil_hash_func_fnv32(void *key)
{
	int len = strlen((char *)key);
	return (size_t)fnv_32_buf(key, len, FNV1_32_INIT);
}

int solUtil_string_equals(void *k1, void *k2)
{
	return strcmp((char*)k1, (char*)k2);
}
/*
int solUtil_char_equals(void *k1, void *k2)
{
	if (((char*)k1)[0] == ((char*)k2)[0]) {
		return 0;
	}
	return 1;
}
*/

SolSet* solSet_new_and_init()
{
	SolSet *s = solSet_new();
	size_t (*f1)(void*) = &solUtil_hash_func_murmur;
	size_t (*f2)(void*) = &solUtil_hash_func_fnv32;
	solSet_set_hash_func1(s, f1);
	solSet_set_hash_func2(s, f2);
	solSet_set_equal_func(s, &solUtil_string_equals);
	return s;
}

/*
SolSet* solSet_for_char_new_and_init()
{
	SolSet *s = solSet_new();
	size_t (*f1)(void*) = &solUtil_hash_func_murmur;
	size_t (*f2)(void*) = &solUtil_hash_func_fnv32;
	solSet_set_hash_func1(s, &f1);
	solSet_set_hash_func2(s, &f2);
	solSet_set_equal_func(s, &solUtil_char_equals);
	return s;
}
*/

SolHash* solHash_new_and_init()
{
	SolHash *h = sol_hash_new();
	solHash_set_funcs_for_string(h);
	return h;
}

inline void solHash_set_funcs_for_string(SolHash *h)
{
	size_t (*f1)(void*) = solUtil_hash_func_murmur;
	size_t (*f2)(void*) = solUtil_hash_func_fnv32;
	sol_hash_set_hash_func1(h, f1);
	sol_hash_set_hash_func2(h, f2);
	sol_hash_set_equal_func(h, &solUtil_string_equals);
}
