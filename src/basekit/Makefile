CC = cc
CFLAGS = -Wall -g -D__DEBUG__

all: sol_list.o sol_hash.o sol_set.o sol_stack.o

sol_list.o: sol_list.c sol_common.h
sol_hash.o: sol_hash.c sol_common.h
sol_set.o: sol_set.c sol_hash.o sol_common.h
sol_stack.o: sol_stack.c sol_list.o sol_common.h

test_hash: test_hash.c sol_hash.o Hash_fnv.c  Hash_murmur.c
test_set: test_set.c sol_set.o sol_hash.o Hash_fnv.c  Hash_murmur.c
test_list: test_list.c sol_list.o
test_stack: test_stack.c sol_stack.o sol_list.o

.PHONY: clean
clean:
	-rm -rf output *.o *.gch test_hash test_set test_list test_stack
