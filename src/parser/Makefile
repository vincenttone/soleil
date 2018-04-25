CC = cc
CFLAGS = -Wall -g -D_SOL_DEBUG__ -I ../basekit
VPATH :=../basekit:test

all: sol_dfa.o sol_pattern.o sol_ll1.o

sol_dfa.o: sol_dfa.c sol_common.h sol_hash.o sol_set.o
sol_pattern.o: sol_pattern.c sol_dfa.o sol_list.o
sol_ll1.o: sol_ll1.c sol_common.h sol_hash.o sol_list.o sol_stack.o sol_rbtree.o sol_rbtree_iter.o
sol_lr.o: sol_lr.c sol_common.h sol_stack.o sol_rbtree.o sol_list.o sol_rbtree_iter.o sol_dl_list.o
sol_slr.o: sol_slr.c sol_common.h  sol_lr.o sol_hash.o sol_list.o sol_stack.o sol_rbtree.o sol_rbtree_iter.o sol_rb_tuple.o
sol_pda.o: sol_pda.c sol_common.h sol_list.o sol_table_fixed.o sol_hash.o
sol_lisp_parser.o: sol_lisp_parser.c sol_pda.o sol_common.h sol_list.o sol_table_fixed.o sol_hash.o

.PHONY: clean
clean:
	-rm -rf output *.o *.gch
