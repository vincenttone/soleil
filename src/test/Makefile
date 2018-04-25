CC = cc
CFLAGS = -Wall -g -D_SOL_DEBUG__ -I ../basekit -I ../parser
VPATH :=../basekit:../parser

hash_files = Hash_fnv.c Hash_murmur.c

test_hash: sol_hash.o $(hash_files)
test_set: sol_set.o sol_hash.o $(hash_files)
test_stack: sol_stack.o sol_dl_list.o
test_rbtree: sol_rbtree.o sol_rbtree_iter.o sol_stack.o sol_dl_list.o
test_rb_tuple: sol_rb_tuple.o sol_rbtree.o sol_list.o

test_dfa: sol_dfa.o sol_common.h sol_hash.o sol_set.o sol_utils.o  $(hash_files)
test_pattern: sol_pattern.o sol_dfa.o sol_hash.o sol_set.o sol_utils.o sol_list.o $(hash_files)
test_ll1: sol_ll1.o sol_stack.o sol_list.o sol_hash.o sol_dl_list.o sol_rbtree.o sol_rbtree_iter.o
test_slr: sol_slr.o sol_lr.o sol_lr.o sol_hash.o sol_list.o sol_stack.o sol_rbtree.o sol_rbtree_iter.o sol_rb_tuple.o sol_dl_list.o
test_pda: sol_pda.o sol_list.o sol_table_fixed.o sol_stack.o sol_dl_list.o sol_hash.o
test_lisp_parser: sol_lisp_parser.o sol_pda.o sol_list.o sol_table_fixed.o sol_stack.o sol_dl_list.o sol_hash.o

test_%: test_%.c sol_%.o
	if [ ! -d output ]; then mkdir output; fi
	$(CC) $(CFLAGS) -o output/$@ $^

.PHONY: clean
clean:
	-rm -rf output *.o *.gch *.dSYM
