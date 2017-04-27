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

void free_echo(void *v)
{
	printf("try to free %s\n", (char*)v);
}

void* test_dup(void *v)
{
	printf("try dup %s\n", (char*)v);
	return v;
}

int main()
{
	SolHash *hash = solHash_new();
	size_t (*f1)(void*) = &hash_func_murmur;
	size_t (*f2)(void*) = &hash_func_fnv32;
	solHash_set_hash_func1(hash, f1);
	solHash_set_hash_func2(hash, f2);
	solHash_set_equal_func(hash, &equals);
	solHash_set_free_k_func(hash, &free_echo);
	solHash_set_free_v_func(hash, &free_echo);
	solHash_put_key_and_val(hash, "key1", "value1");
	solHash_put_key_and_val(hash, "key2", "value2");
	solHash_put_key_and_val(hash, "key3", "value3");
	solHash_put_key_and_val(hash, "key4", "value4");
	solHash_put_key_and_val(hash, "key5", "value5");
	solHash_put_key_and_val(hash, "key6", "value6");
	solHash_put_key_and_val(hash, "key7", "value7");
	solHash_put_key_and_val(hash, "key8", "value8");
	solHash_put_key_and_val(hash, "key9", "value9");
	printf("count is %d\n", (int)solHash_count(hash));
	printf("hash size is %d\n", (int)hash->size);
	printf("value of key1 is %s\n", (char*)solHash_find_value(hash, "key1"));
	printf("value of key2 is %s\n", (char*)solHash_find_value(hash, "key2"));
	printf("value of key3 is %s\n", (char*)solHash_find_value(hash, "key3"));
	size_t i = 0;
	SolHashRecord *r;
	SolHashIter *iter = solHashIter_new(hash);
	do {
		r = solHashIter_current_record(iter);
		if (r == NULL) {
			printf("ITER GOT(%d):\tNULL\n", (int)i);
		} else if (r->k == NULL) {
			printf("ITER GOT(%d):\tempty data\n", (int)i);
		} else {
			printf("ITER GOT(%d):\t%s --> %s\n", (int)i, (char*)r->k, (char*)r->v);
		}
		solHashIter_next(iter);
	} while (++i < hash->size);
	solHash_remove_by_key(hash, "key3");
	printf("remove key key3\n");
	solHashIter_rewind(iter);
	i = 0;
	do {
		r = solHashIter_current_record(iter);
		if (r == NULL) {
			printf("ITER GOT(%d):\tNULL\n", (int)i);
		} else if (r->k == NULL) {
			printf("ITER GOT(%d):\tempty data\n", (int)i);
		} else {
			printf("ITER GOT(%d):\t%s --> %s\n", (int)i, (char*)r->k, (char*)r->v);
		}
		solHashIter_next(iter);
	} while (++i < hash->size);
	// test dup
	SolHash *hash1 = solHash_new();
	solHash_dup(hash1, hash);
	SolHashIter *iter1 = solHashIter_new(hash);
	i = 0;
	do {
		r = solHashIter_current_record(iter1);
		if (r == NULL) {
			printf("DUPED HASH ITER GOT(%d):\tNULL\n", (int)i);
		} else if (r->k == NULL) {
			printf("DUPED HASH ITER GOT(%d):\tempty data\n", (int)i);
		} else {
			printf("DUPED HASH ITER GOT(%d):\t%s --> %s\n", (int)i, (char*)r->k, (char*)r->v);
		}
		solHashIter_next(iter1);
	} while (++i < hash->size);
	solHash_set_dup_k_func(hash, &test_dup);
	solHash_set_dup_k_func(hash, &test_dup);
	// test dup
	SolHash *hash2 = solHash_new();
	solHash_dup(hash2, hash);
	SolHashIter *iter2 = solHashIter_new(hash);
	i = 0;
	do {
		r = solHashIter_current_record(iter2);
		if (r == NULL) {
			printf("DUPED 2 HASH ITER GOT(%d):\tNULL\n", (int)i);
		} else if (r->k == NULL) {
			printf("DUPED 2 HASH ITER GOT(%d):\tempty data\n", (int)i);
		} else {
			printf("DUPED 2 HASH ITER GOT(%d):\t%s --> %s\n", (int)i, (char*)r->k, (char*)r->v);
		}
		solHashIter_next(iter2);
	} while (++i < hash->size);
	

	solHashIter_free(iter);
	solHashIter_free(iter1);
	solHashIter_free(iter2);
	solHash_free(hash);
	solHash_free(hash1);
	solHash_free(hash2);
	return 0;
}
