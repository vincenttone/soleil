#include <stdlib.h>
#include <stdio.h>
#include "sol_rbtree.h"

#define conv_val(v) (*(int*)v)
#define conv_node_val(n) conv_val(solRBTreeNode_val(n))

int print_node_and_children(SolRBTree *tree, SolRBTreeNode *node)
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

int print_key(SolRBTree *tree, SolRBTreeNode *node)
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

int cmp(void *v1, void *v2)
{
    if (conv_val(v1) == conv_val(v2)) return 0;
    if (conv_val(v1) < conv_val(v2)) return -1;
    return 1;
}

int main()
{
    const int CLEN = 12;
    SolRBTree *tree = solRBTree_new();
    solRBTree_set_compare_func(tree, &cmp);
    solRBTree_set_val_free_func(tree, &print_free_val);
    int counts[CLEN];
    int i;
    for (i = 1; i <= CLEN; i++) {
        counts[i] = i;
        solRBTree_insert(tree, &counts[i]);
    }
    printf("ROOT is %d\n", conv_val(solRBTreeNode_val(solRBTree_root(tree))));
    printf("---preorder travelsal---\n");
    solRBTree_travelsal_preorder(tree, solRBTree_root(tree), &print_key);
    printf("---inorder travelsal---\n");
    solRBTree_travelsal_inorder(tree, solRBTree_root(tree), &print_node_and_children);
    printf("---backorder travelsal---\n");
    solRBTree_travelsal_backorder(tree, solRBTree_root(tree), &print_key);
    printf("MAX is %d\n", conv_val(solRBTree_max(tree)));
    printf("MIN is %d\n", conv_val(solRBTree_min(tree)));
    SolRBTreeNode *n = solRBTree_search_node(tree, &counts[CLEN - 2]);
    i = conv_node_val(n);
    printf("delete node %d, result %d\n", i, solRBTree_delete_node(tree, n));
    i = conv_node_val(solRBTree_root(tree));
    printf("ROOT is %d\n", i);
    printf("---preorder travelsal after delete node---\n");
    solRBTree_travelsal_preorder(tree, solRBTree_root(tree), &print_node_and_children);
    printf("delete node %d, result %d\n", i, solRBTree_del(tree, &counts[i]));
    printf("ROOT is %d\n", conv_node_val(solRBTree_root(tree)));
    printf("---preorder travelsal after delete node---\n");
    solRBTree_travelsal_preorder(tree, solRBTree_root(tree), &print_node_and_children);
    solRBTree_free(tree);
    return 0;
}
