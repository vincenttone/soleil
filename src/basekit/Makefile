CC = cc
CFLAGS = -Wall -g -D__DEBUG__

all: list.o sol_hash.o sol_set.o sol_stack.o sol_utils.o

list.o: list.c
sol_hash.o: sol_hash.c sol_common.h
sol_set.o: sol_set.c sol_hash.o sol_common.h
sol_stack.o: sol_stack.c list.o sol_common.h
sol_utils.o: sol_utils.c sol_common.h

.PHONY: clean
clean:
	-rm -rf output *.o *.gch
