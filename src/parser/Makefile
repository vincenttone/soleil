CC = cc
CFLAGS = -Wall -g -D__DEBUG__ -I ../basekit
VPATH :=../basekit:test

all: sol_nfa.o sol_dfa.o sol_pattern.o

sol_nfa.o: sol_nfa.c sol_common.h sol_set.o sol_hash.o sol_utils.o Hash_fnv.c Hash_murmur.c
sol_dfa.o: sol_dfa.c sol_common.h sol_hash.o
sol_pattern.o: sol_pattern.c sol_nfa.o

test_dfa: test_dfa.c sol_dfa.o sol_common.h sol_hash.o sol_utils.o  Hash_fnv.c Hash_murmur.c
	if [ ! -d output ]; then mkdir output; fi
	$(CC) $(CFLAGS) -o output/$@ $^

test_nfa: test_nfa.c sol_nfa.o sol_common.h sol_set.o sol_hash.o sol_utils.o  Hash_fnv.c Hash_murmur.c
	if [ ! -d output ]; then mkdir output; fi
	$(CC) $(CFLAGS) -o output/$@ $^

test_pattern: test_pattern.c sol_pattern.o sol_nfa.o sol_set.o sol_hash.o sol_utils.o Hash_fnv.c Hash_murmur.c
	if [ ! -d output ]; then mkdir output; fi
	$(CC) $(CFLAGS) -o output/$@ $^

.PHONY: clean
clean:
	-rm -rf output *.o *.gch
