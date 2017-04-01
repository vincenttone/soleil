CC = cc
CFLAGS = -Wall -g -D__DEBUG__ -I ../basekit
VPATH :=../basekit:test

all: sol_dfa.o sol_nfa.o

sol_fa.o: sol_fa.c sol_common.h list.o sol_utils.o
sol_dfa.o: sol_dfa.c sol_common.h sol_fa.o sol_utils.o
sol_nfa.o: sol_nfa.c sol_common.h sol_fa.o sol_utils.o sol_set.o sol_hash.o list.o sol_fa.o
test_dfa: test_dfa.c sol_common.h list.o sol_dfa.o sol_fa.o
	if [ ! -d output ]; then mkdir output; fi
	$(CC) $(CFLAGS) -o output/$@ $^

test_nfa: test_nfa.c sol_nfa.o sol_common.h sol_set.o sol_hash.o list.o sol_utils.o sol_fa.o Hash_fnv.c Hash_murmur.c
	if [ ! -d output ]; then mkdir output; fi
	$(CC) $(CFLAGS) -o output/$@ $^

.PHONY: clean
clean:
	-rm -rf output *.o *.gch
