#ifndef _SOL_LR_
#define _SOL_LR_ 1

#include <stdarg.h>
#include "sol_common.h"
#include "sol_rbtree.h"
#include "sol_list.h"

#define SolLRSymbolFlag_ORIGIN               0x1
#define SolLRSymbolFlag_START                0x2
#define SolLRSymbolFlag_END                  0x4
#define SolLRSymbolFlag_TERMINAL             0x10
#define SolLRSymbolFlag_NONTERMINAL          0x20
#define SolLRSymbolFlag_EMPTY                0x40
#define SolLRSymbolFlag_NULLABLE             0x50
#define SolLRSymbolFlag_COMPUTING            0x100
#define SolLRSymbolFlag_NULLABLE_COMPUTED    0x200
#define SolLRSymbolFlag_FIRST_COMPUTED       0x300
#define SolLRSymbolFlag_FOLLOW_COMPUTED      0x400

#define SolLRItemCol_FLAG_END  0x1
#define SolLRItemCol_FLAG_BUSY  0x2

#define SolLRParserItemCol_INIT_SIZE 32

typedef struct _SolLRSymbol {
  int flag;   // flag
  void *v; // symbol val
  SolList *productions; // productions
  SolRBTree *firsts;
  SolRBTree *follows;
} SolLRSymbol;

typedef struct _SolLRProduct {
  size_t len; // len
  SolLRSymbol *s;
  SolLRSymbol **r; // right
} SolLRProduct;

typedef struct _SolLRItem {
  size_t pos;
  SolLRProduct *p;
} SolLRItem;

typedef struct _SolLRItemCol { // items collection
  size_t state; // state
  size_t flag; // flag
  SolList *items; // items
  SolRBTree *nc; // next items collection
  SolLRSymbol *sym; // pre symbol
} SolLRItemCol;

typedef struct _SolLRParser {
  size_t gen; // state generate
  SolList *collections; // items collection
  int (*compare_symbol_and_col)(void*, void*);
  int (*compare_cols)(void*, void*);
} SolLRParser;

SolLRParser* solLRParser_new();
void solLRParser_free(SolLRParser*);
SolLRItemCol* solLRParser_generate_items_collection(SolLRParser*);

SolLRSymbol* solLRSymbol_new(void*);
void solLRSymbol_free(SolLRSymbol*);
void _solLRSymbol_free(void*);

SolLRSymbol* solLRSymbol_terminal_new(void*);
SolLRSymbol* solLRSymbol_nonterminal_new(void*);

SolLRProduct* solLRProduct_new(SolLRSymbol*, size_t, ...);
void solLRProduct_free(SolLRProduct*);
void _solLRProduct_free(void*);

SolLRItem* solLRItem_new(SolLRProduct*, size_t);
void solLRItem_free(SolLRItem*);

SolLRItemCol* solLRItemCol_new();
void solLRItemCol_free(SolLRItemCol*);
void _solLRItemCol_free(void*);

int solLRSymbol_record_first(SolLRSymbol*, SolLRSymbol*);
int solLRSymbol_record_follow(SolLRSymbol*, SolLRSymbol*);

int _solLRSymbol_share_firsts(SolRBTree*, SolRBTreeNode*, void*);
int _solLRSymbol_share_follows(SolRBTree*, SolRBTreeNode*, void*);

int solLRSymbol_compute_nullable(SolLRSymbol*);
int solLRSymbol_compute_first(SolLRSymbol*, SolLRSymbol*);
int solLRSymbol_compute_follow(SolLRSymbol*, SolRBTree*, SolLRSymbol*);

int solLRParser_compute_items_collections(SolLRParser*, SolLRItemCol*);
int solLRParser_compute_nonkernel_items(SolLRParser*, SolLRItemCol*, SolLRSymbol*);

int _solLRParser_compute_items_collections(SolRBTree*, SolRBTreeNode*, void*);
void _solLRItem_free(void*);

#define solLRSymbol_set_flag(s, f) ((s)->flag |= f)

#define solLRSymbol_is_terminal(s)   ((s)->flag & SolLRSymbolFlag_TERMINAL)
#define solLRSymbol_is_nonterminal(s) ((s)->flag & SolLRSymbolFlag_NONTERMINAL)
#define solLRSymbol_is_origin(s)   ((s)->flag & SolLRSymbolFlag_ORIGIN)

#define solLRSymbol_is_nullable(s) ((s)->flag & SolLRSymbolFlag_NULLABLE)
#define solLRSymbol_set_nullable(s) ((s)->flag |= SolLRSymbolFlag_NULLABLE)

#define solLRSymbol_set_nullable_computed(s) ((s)->flag |= SolLRSymbolFlag_NULLABLE_COMPUTED)
#define solLRSymbol_nullable_computed(s) ((s)->flag & SolLRSymbolFlag_NULLABLE_COMPUTED)

#define solLRSymbol_set_first_computed(s) ((s)->flag |= SolLRSymbolFlag_FIRST_COMPUTED)
#define solLRSymbol_first_computed(s) ((s)->flag & SolLRSymbolFlag_FIRST_COMPUTED)

#define solLRSymbol_set_follow_computed(s) ((s)->flag |= SolLRSymbolFlag_FOLLOW_COMPUTED)
#define solLRSymbol_follow_computed(s) ((s)->flag & SolLRSymbolFlag_FOLLOW_COMPUTED)

#define solLRSymbol_is_computing(s) ((s)->flag & SolLRSymbolFlag_COMPUTING)
#define solLRSymbol_is_idle(s) (((s)->flag & SolLRSymbolFlag_COMPUTING) == 0)
#define solLRSymbol_set_is_busy(s) (s)->flag |= SolLRSymbolFlag_COMPUTING
#define solLRSymbol_set_is_idle(s) (s)->flag = ((s)->flag) & (~SolLRSymbolFlag_COMPUTING)

#define solLRSymbol_copy_firsts(f, symbol) solRBTree_travelsal_inorder(f, \
                                                                       solRBTree_root(f), \
                                                                       &_solLRSymbol_share_firsts, \
                                                                       symbol)
#define solLRSymbol_copy_follows(f, symbol) solRBTree_travelsal_inorder(f, \
                                                                        solRBTree_root(f), \
                                                                        &_solLRSymbol_share_follows, \
                                                                        symbol)

#define solLRProduct_left(p) (p)->s
#define solLRProduct_right(p) (p)->r
#define solLRProduct_size(p) (p)->l
#define solLRProduct_find_symbol(p, s)  (SolLRSymbol**)((p)->r + s)

#endif
