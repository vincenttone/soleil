CC = cc
CFLAGS = -Wall -g -D__DEBUG__

all: sol_dl_list.o sol_hash.o sol_set.o sol_stack.o sol_utils.o sol_list.o \
	sol_rbtree.o sol_rbtree_iter.o sol_sparse_matrix.o

sol_dl_list.o: sol_dl_list.c sol_common.h
sol_list.o: sol_list.c sol_common.h
sol_hash.o: sol_hash.c sol_common.h
sol_set.o: sol_set.c sol_hash.o sol_common.h
sol_stack.o: sol_stack.c sol_dl_list.o sol_common.h
sol_utils.o: sol_utils.c
sol_rbtree.o: sol_rbtree.c sol_common.h
sol_rbtree_iter.o: sol_rbtree_iter.c sol_rbtree.o sol_stack.o sol_dl_list.o sol_common.h
sol_rb_tuple.o: sol_rb_tuple.c sol_rbtree.c sol_common.h sol_list.h
sol_sparse_matrix.o: sol_sparse_matrix.c sol_common.h
sol_table_fixed.o: sol_table_fixed.c sol_common.h

.PHONY: clean
clean:
	-rm -rf output *.o *.gch *.dSYM
