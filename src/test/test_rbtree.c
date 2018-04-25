#include <stdlib.h>
#include <stdio.h>
#include "sol_rbtree.h"
#include "sol_rbtree_iter.h"

#define conv_val(v) (*(int*)v)
#define conv_node_val(n) conv_val(solRBTreeNode_val(n))

int print_node_and_children(SolRBTree *tree, SolRBTreeNode *node, void *d)
{
    if (solRBTree_node_is_nil(tree, node)) {
        printf("NIL\n");
        return 0;
    }
    char* colors[_SolRBTreeCol_black + 1];
    colors[_SolRBTreeCol_red] = "red";
    colors[_SolRBTreeCol_black] = "black";
    SolRBTreeNode *ln = solRBTreeNode_left(node);
    SolRBTreeNode *rn = solRBTreeNode_right(node);
    int lv = solRBTree_node_is_nil(tree, ln) ? 0 : conv_val(solRBTreeNode_val(ln));
    int rv = solRBTree_node_is_nil(tree, rn) ? 0 : conv_val(solRBTreeNode_val(rn));
    printf("node: %d (%s), left: %d (%s), right: %d (%s)\n",
           conv_val(solRBTreeNode_val(node)),
           colors[solRBTreeNode_color(node)],
           lv,
           colors[solRBTreeNode_color(ln)],
           rv,
           colors[solRBTreeNode_color(rn)]
        );
    return 0;
}

int print_key(SolRBTree *tree, SolRBTreeNode *node, void *d)
{
    char* colors[_SolRBTreeCol_black + 1];
    colors[_SolRBTreeCol_red] = "red";
    colors[_SolRBTreeCol_black] = "black";
    if (solRBTreeNode_val(node)) {
        printf("val is: %d (%s)\n", *(int*)solRBTreeNode_val(node), colors[solRBTreeNode_color(node)]);
        return 0;
    } else {
        printf("empty value\n");
        return 1;
    }
}

void print_free_val(void *val)
{
    printf("Free node of val: %d\n", *(int*)val);
}

int cmp(void *v1, void *v2, SolRBTree *tree, int flag)
{
    if (flag & SolRBTree_CMP_FLAG_INSERT) {
        printf("insert compare %d\n", conv_val(v1));
    } else if (flag & SolRBTree_CMP_FLAG_COMPARE) {
        printf("compare's compare %d\n", conv_val(v1));
    }
    if (conv_val(v1) == conv_val(v2)) return 0;
    if (conv_val(v1) < conv_val(v2)) return -1;
    return 1;
}

int conflict_fix(void *v1, void *v2)
{
    printf("insert conflict %d\n", conv_val(v1));
    return 0;
}

int main()
{
    const int CLEN = 12;
    SolRBTree *tree = solRBTree_new();
    solRBTree_set_compare_func(tree, &cmp);
    solRBTree_set_val_free_func(tree, &print_free_val);
    SolRBTree *tree1 = solRBTree_new();
    solRBTree_set_compare_func(tree1, &cmp);
    solRBTree_set_val_free_func(tree1, &print_free_val);
    int counts[CLEN];
    int i;
    for (i = 1; i <= CLEN; i++) {
        counts[i] = i;
        solRBTree_insert(tree, &counts[i]);
        solRBTree_insert(tree1, &counts[i]);
    }
    printf("compare tree return %d\n", solRBTree_compare_tree(tree, solRBTree_root(tree), tree1, solRBTree_root(tree1)));
    solRBTree_insert(tree, &counts[3]);
    printf("compare tree return %d\n", solRBTree_compare_tree(tree, solRBTree_root(tree), tree1, solRBTree_root(tree1)));
    printf("tree ROOT is %d, count: %zu\n", conv_val(solRBTreeNode_val(solRBTree_root(tree))), solRBTree_count(tree));
    printf("tree1 ROOT is %d, count: %zu\n", conv_val(solRBTreeNode_val(solRBTree_root(tree1))), solRBTree_count(tree1));
    printf("---preorder travelsal---\n");
    solRBTree_travelsal_preorder(tree, solRBTree_root(tree), &print_key, NULL);
    printf("---inorder travelsal---\n");
    solRBTree_travelsal_inorder(tree, solRBTree_root(tree), &print_node_and_children, NULL);
    printf("---backorder travelsal---\n");
    solRBTree_travelsal_backorder(tree, solRBTree_root(tree), &print_key, NULL);
    printf("MAX is %d\n", conv_val(solRBTree_max(tree)));
    printf("MIN is %d\n", conv_val(solRBTree_min(tree)));
    SolRBTreeNode *n = solRBTree_search_node(tree, &counts[CLEN - 2]);
    i = conv_node_val(n);
    printf("delete node %d, result %d\n", i, solRBTree_delete_node(tree, n));
    i = conv_node_val(solRBTree_root(tree));
    // printf("ROOT is %d\n", i);
    printf("ROOT is %d, count: %zu\n", conv_val(solRBTreeNode_val(solRBTree_root(tree))), solRBTree_count(tree));
    printf("---preorder travelsal after delete node---\n");
    solRBTree_travelsal_preorder(tree, solRBTree_root(tree), &print_node_and_children, NULL);
    printf("delete node %d, result %d\n", i, solRBTree_del(tree, &counts[i]));
    printf("ROOT is %d, count: %zu\n", conv_node_val(solRBTree_root(tree)), solRBTree_count(tree));
    printf("---preorder travelsal after delete node---\n");
    solRBTree_travelsal_preorder(tree, solRBTree_root(tree), &print_node_and_children, NULL);
    printf("---------Test iter preorder--------\n");
    SolRBTreeIter *ip = solRBTreeIter_new(tree, solRBTree_root(tree), SolRBTreeIterTT_preorder);
    do {
        n = solRBTreeIter_current(ip);
        printf("stack size %zu\t", solStack_size(ip->s));
        print_key(tree, n, NULL);
    } while (solRBTreeIter_next(ip));
    solRBTreeIter_free(ip);
    printf("---------End test iter preorder--------\n");
    printf("---------Test iter inorder--------\n");
    SolRBTreeIter *ii = solRBTreeIter_new(tree, solRBTree_root(tree), SolRBTreeIterTT_inorder);
    do {
        n = solRBTreeIter_current(ii);
        printf("stack size %zu\t", solStack_size(ii->s));
        print_key(tree, n, NULL);
    } while (solRBTreeIter_next(ii));
    solRBTreeIter_free(ii);
    printf("---------End test iter inorder--------\n");
    printf("---------Test iter backorder--------\n");
    SolRBTreeIter *ib = solRBTreeIter_new(tree, solRBTree_root(tree), SolRBTreeIterTT_backorder);
    do {
        n = solRBTreeIter_current(ib);
        printf("stack size %zu\t", solStack_size(ib->s));
        print_key(tree, n, NULL);
    } while (solRBTreeIter_next(ib));
    solRBTreeIter_free(ib);
    printf("---------End test iter backorder--------\n");
    printf("compare tree return %d\n", solRBTree_compare_tree(tree, solRBTree_root(tree), tree1, solRBTree_root(tree1)));
    solRBTree_free(tree);
    solRBTree_free(tree1);
    return 0;
}
