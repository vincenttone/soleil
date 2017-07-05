CC = cc
CFLAGS = -Wall -g -D_SOL_DEBUG__ -I ../basekit
VPATH :=../basekit:test

all: sol_dfa.o sol_pattern.o sol_ll1.o

sol_dfa.o: sol_dfa.c sol_common.h sol_hash.o sol_set.o
sol_pattern.o: sol_pattern.c sol_dfa.o sol_list.o
sol_ll1.o: sol_ll1.c sol_common.h sol_hash.o sol_list.o sol_stack.o sol_rbtree.o sol_rbtree_iter.o

test_dfa: test_dfa.c sol_dfa.o sol_common.h sol_hash.o sol_set.o sol_utils.o  Hash_fnv.c Hash_murmur.c
	if [ ! -d output ]; then mkdir output; fi
	$(CC) $(CFLAGS) -o output/$@ $^

test_pattern: test_pattern.c sol_pattern.o sol_dfa.o sol_hash.o sol_set.o sol_utils.o sol_list.o Hash_fnv.c Hash_murmur.c
	if [ ! -d output ]; then mkdir output; fi
	$(CC) $(CFLAGS) -o output/$@ $^

test_ll1: test_ll1.c sol_ll1.o sol_stack.o sol_list.o sol_hash.o sol_dl_list.o sol_rbtree.o sol_rbtree_iter.o
	if [ ! -d output ]; then mkdir output; fi
	$(CC) $(CFLAGS) -o output/$@ $^

.PHONY: clean
clean:
	-rm -rf output *.o *.gch
