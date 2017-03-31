#include <stdio.h>
#include <string.h>
#include "sol_nfa.h"
#include "sol_set.h"
#include "Hash_fnv.h"
#include "Hash_murmur.h"

size_t hash_func_murmur(void*);
size_t hash_func_fnv32(void*);
int equals(void *, void*);

size_t hash_func_murmur(void *key)
{
	int len = strlen((char *)key);
	return MurmurHash2(key, len, 0);
}

size_t hash_func_fnv32(void *key)
{
	int len = strlen((char *)key);
	return (size_t)fnv_32_buf(key, len, FNV1_32_INIT);
}

int equals(void *k1, void *k2)
{
	return strcmp((char *)k1, (char *)k2);
}

int main()
{
	SolSet *cs = solSet_new();
	SolSet *as = solSet_new();

	size_t (*f1)(void*) = &hash_func_murmur;
	size_t (*f2)(void*) = &hash_func_fnv32;

	solSet_set_hash_func1(cs, f1);
	solSet_set_hash_func2(cs, f2);
	solSet_set_equal_func(cs, &equals);

	solSet_set_hash_func1(as, f1);
	solSet_set_hash_func2(as, f2);
	solSet_set_equal_func(as, &equals);

	
	
	return 0;
}
