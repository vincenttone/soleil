CC = cc
CFLAGS = -Wall -g -D__DEBUG__

all: list.o sol_hash.o

list.o: list.c
sol_hash.o: sol_hash.c sol_common.h

.PHONY: clean
clean:
	-rm -rf output *.o *.gch
