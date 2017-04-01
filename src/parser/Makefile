CC = cc
CFLAGS = -Wall -g -D__DEBUG__ -I ../basekit
VPATH :=../basekit:test

all: sol_dfa.o sol_nfa.o

sol_fa.o: sol_fa.c sol_common.h list.o
sol_dfa.o: sol_dfa.c sol_common.h sol_fa.o list.o
sol_nfa.o: sol_nfa.c sol_common.h sol_common.c sol_fa.o
test: test_dfa.c sol_dfa.o sol_fa.o list.o
	if [ ! -d output ]; then mkdir output; fi
	$(CC) $(CFLAGS) -o output/$@ $^

.PHONY: clean
clean:
	-rm -rf output *.o *.gch
