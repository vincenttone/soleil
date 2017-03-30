CC = cc
CFLAGS = -Wall -g -D__DEBUG__

all: list.o sol_hash.o sol_set.o

list.o: list.c
sol_hash.o: sol_hash.c sol_common.h
sol_set.o: sol_set.c sol_hash.o

.PHONY: clean
clean:
	-rm -rf output *.o *.gch
