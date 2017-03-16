CC = cc
CFLAGS = -Wall -g -D__DEBUG__ -I ../basekit
VPATH :=../basekit:test

all: sol_dfa.o

sol_fa.o: sol_fa.h sol_common.h
sol_dfa.o: sol_dfa.h sol_common.h sol_fa.o
test: test_dfa.c sol_dfa.o sol_fa.o list.c
	if [ ! -d output ]; then mkdir output; fi
	$(CC) $(CFLAGS) -o output/$@ $^

.PHONY: clean
clean:
	-rm -rf output *.o *.gch
