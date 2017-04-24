CC = cc
CFLAGS = -Wall -g -D__DEBUG__ -I ../basekit
VPATH :=../basekit:test

all: sol_nfa.o

sol_nfa.o: sol_nfa.c sol_common.h sol_set.o sol_hash.o sol_utils.o Hash_fnv.c Hash_murmur.c

test_nfa: test_nfa.c sol_nfa.o sol_common.h sol_set.o sol_hash.o sol_utils.o  Hash_fnv.c Hash_murmur.c
	if [ ! -d output ]; then mkdir output; fi
	$(CC) $(CFLAGS) -o output/$@ $^

.PHONY: clean
clean:
	-rm -rf output *.o *.gch
