#include <stdio.h>
#include <string.h>
#include "sol_hash.h"
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
	SolSet *s = solSet_new();
	size_t (*f1)(void*) = &hash_func_murmur;
	size_t (*f2)(void*) = &hash_func_fnv32;
	solSet_set_hash_func1(s, f1);
	solSet_set_hash_func2(s, f2);
	solSet_set_equal_func(s, &equals);
	solSet_add(s, "value1");
	solSet_add(s, "value1");
	solSet_add(s, "value2");
	solSet_add(s, "value2");
	solSet_add(s, "value3");
	solSet_add(s, "value4");
	solSet_add(s, "value5");
	solSet_add(s, "value5");
	printf("set size: %d\n", (int)solSet_size(s));
	printf("set length: %d\n", (int)solSet_count(s));
	printf("value10 is in set?\t%d\n", solSet_in_set(s, "value10"));
	printf("value3 is in set?\t%d\n", solSet_in_set(s, "value3"));
	void *c;
	while ((c = solSet_get_value(s))) {
		printf("Got:\t%s\n", (char *)c);
		// printf("Set size: %d\t, iter num: %d\n", (int)solSet_size(s), (int)s->iter->num);
		solSet_next(s);
	}
	solSet_free(s);
	return 0;
}
