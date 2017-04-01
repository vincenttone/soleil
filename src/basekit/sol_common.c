#include "sol_common.h"

size_t solCommon_hash_func_murmur(void *key)
{
	int len = strlen((char *)key);
	return MurmurHash2(key, len, 0);
}

size_t solCommon_hash_func_fnv32(void *key)
{
	int len = strlen((char *)key);
	return (size_t)fnv_32_buf(key, len, FNV1_32_INIT);
}

int solCommon_string_equals(void *k1, void *k2)
{
	return strcmp((char *)k1, (char *)k2);
}

int solCommon_char_equals(void *k1, void *k2)
{
	if ((char)k1 == (cha)k2) {
		return 0;
	}
	return 1;
}

SolSet* solSet_new_and_init()
{
	solSet *s = solSet_new();
	solSet_set_hash_func1(s, (size_t*)solCommon_hash_func_murmur);
	solSet_set_hash_func2(s, (size_t*)solCommon_hash_func_fnv32);
	solSet_set_equal_func(s, (int*)solCommon_string_equals);
	return s;
}

SolSet* solSet_for_char_new_and_init()
{
	solSet *s = solSet_new();
	solSet_set_hash_func1(s, (size_t*)solCommon_hash_func_murmur);
	solSet_set_hash_func2(s, (size_t*)solCommon_hash_func_fnv32);
	solSet_set_equal_func(s, (int*)solCommon_char_equals);
	return s;
}

SolHash* solHash_new_and_init()
{
	solSet *h = hash_new();
	solSet_set_hash_func1(h, (size_t*)solCommon_hash_func_murmur);
	solSet_set_hash_func2(h, (size_t*)solCommon_hash_func_fnv32);
	solSet_set_equal_func(h, (int*)solCommon_equals);
	return h;
}
