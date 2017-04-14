CC = cc
CFLAGS = -Wall -g -D__DEBUG__ -I ../basekit
VPATH :=../basekit:test

all: sol_pda.o

sol_pda.o: sol_pda.c sol_common.h sol_set.o sol_hash.o Hash_fnv.c Hash_murmur.c

test_pda: test_pda.c sol_pda.o sol_common.h sol_set.o sol_hash.o
	if [ ! -d output ]; then mkdir output; fi
	$(CC) $(CFLAGS) -o output/$@ $^

.PHONY: clean
clean:
	-rm -rf output *.o *.gch
