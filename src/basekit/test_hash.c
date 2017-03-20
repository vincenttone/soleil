#include <stdio.h>
#include <string.h>
#include "sol_hash.h"
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
	SolHash *hash = sol_hash_new();
	size_t (*f1)(void*) = &hash_func_murmur;
	size_t (*f2)(void*) = &hash_func_fnv32;
	sol_hash_set_hash_func1(hash, f1);
	sol_hash_set_hash_func2(hash, f2);
	sol_hash_set_equal_func(hash, &equals);
	sol_hash_put_key_and_val(hash, "key1", "value1");
	sol_hash_put_key_and_val(hash, "key2", "value2");
	sol_hash_put_key_and_val(hash, "key3", "value3");
	printf("count is %d\n", (int)sol_hash_count(hash));
	printf("value of key1 is %s\n", (char*)sol_hash_find_value(hash, "key1"));
	printf("value of key2 is %s\n", (char*)sol_hash_find_value(hash, "key2"));
	printf("value of key3 is %s\n", (char*)sol_hash_find_value(hash, "key3"));
	sol_hash_free(hash);
}
