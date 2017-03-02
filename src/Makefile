CC = cc
CFLAGS = -Wall -g -D__DEBUG__
VPATH :=libs

all: sol_dfa.o list.o

list.o: list.h
sol_fa.o: sol_common.h sol_fa.h
sol_dfa.o: sol_common.h sol_dfa.h sol_fa.o

test:  sol_fa.o sol_dfa.o list.o test.c
	if [ ! -d output ]; then mkdir output; fi
	$(CC) $(CFLAGS) -o output/$@ $^

.PHONY: clean
clean:
	-rm -rf output *.o
