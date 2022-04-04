//
// Created by cyberpirate on 3/30/22.
//

#include "greatest.h"
#include "oct_tree.h"
#include "linked_stack.h"
#include "rand_gen.h"

TEST rand_pos_test(void) {

    reset_rand();

    for(int i = 0; i < 5; i++) {
        struct Pos pos = rand_pos();
        printf("x: %.02f\ny: %.02f\nz: %.02f\n", pos.x, pos.y, pos.z);
    }

    PASS();
}

TEST oct_node_create(void) {

    struct OctNode* root = create_oct_node();

    ASSERT_EQ(CT_EMPTY, root->contentType);

    destroy_oct_node(root);

    PASS();
}

TEST oct_tree_add(void) {

    struct OctNode* root = create_oct_node();

    int leafNum = NODE_CHILDREN_NUM+1;
    struct Leaf* leaves[leafNum];

    for(int i = 0; i < leafNum; i++) {
        leaves[i] = create_leaf();
        leaves[i]->pos = rand_pos();
        add_leaf(root, leaves[i]);
        if(i < NODE_CHILDREN_NUM) {
            ASSERT_EQ(CT_LEAVES, root->contentType);
        }
    }

    ASSERT_EQ(CT_NODES, root->contentType);
    ASSERT_EQ(NODE_CHILDREN_NUM+1, node_size(root));

    for(int i = 0; i < leafNum; i++) {
        remove_leaf(root, leaves[i]);
        destroy_leaf(leaves[i]);
    }

    destroy_oct_node(root);

    PASS();
}

TEST oct_tree_remove(void) {

    struct OctNode* root = create_oct_node();

    int leafNum = (int) (NODE_CHILDREN_NUM * 1.5);
    struct Leaf* leaves[leafNum];

    for(int i = 0; i < leafNum; i++) {
        leaves[i] = create_leaf();
        leaves[i]->pos = rand_pos();
        add_leaf(root, leaves[i]);
    }

    for(int i = 0; i < leafNum; i++) {
        remove_leaf(root, leaves[i]);
        destroy_leaf(leaves[i]);
    }

    ASSERT_EQ(0, node_size(root));
    ASSERT_EQ(CT_EMPTY, root->contentType);

    destroy_oct_node(root);

    PASS();
}

TEST linked_stack(void) {

    reset_rand();
    const int nodeCnt = 20;

    struct LinkedStackRoot* root = create_linked_stack_root();

    for(int i = 0; i < nodeCnt; i++) {
        struct Leaf* leaf = create_leaf();
        leaf->pos = rand_pos();
        add_node(root, leaf);
    }

    ASSERT_EQ(nodeCnt, root->count);

    for(int i = 0; i < nodeCnt; i++) {
        destroy_leaf(get_and_pop_value(root));
    }

    ASSERT_EQ(0, root->count);
    ASSERT_EQ(nullptr, root->root);

    destroy_linked_stack_root(root);

    PASS();
}

TEST balance_tree_test(void) {
    printf("\n");

    struct OctNode* root = create_oct_node();

    int leafNum = 1000;
    struct Leaf* leaves[leafNum];

    for(int i = 0; i < leafNum; i++) {
        leaves[i] = create_leaf();
        leaves[i]->pos = rand_pos();
        add_leaf(root, leaves[i]);
    }

    ASSERT_EQ(CT_NODES, root->contentType);

    int minUNodeCount = leafNum;
    int maxUNodeCount = 0;
    int minBNodeCount = leafNum;
    int maxBNodeCount = 0;

    for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
        int ns = node_size(root->nodes[i]);
        printf("%d: %d\n", i, ns);
        minUNodeCount = minUNodeCount < ns ? minUNodeCount : ns;
        maxUNodeCount = maxUNodeCount > ns ? maxUNodeCount : ns;
    }

    balance_tree(root);

    for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
        int ns = node_size(root->nodes[i]);
        printf("%d: %d\n", i, ns);
        minBNodeCount = minBNodeCount < ns ? minBNodeCount : ns;
        maxBNodeCount = maxBNodeCount > ns ? maxBNodeCount : ns;
    }

    printf("unbalanced spread: %d\n", maxUNodeCount - minUNodeCount);
    printf("balanced spread  : %d\n", maxBNodeCount - minBNodeCount);

    for(int i = 0; i < leafNum; i++) {
        remove_leaf(root, leaves[i]);
        destroy_leaf(leaves[i]);
    }

    destroy_oct_node(root);

    PASS();
}

/* Add all the definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();      /* command-line arguments, initialization. */

    RUN_TEST(rand_pos_test);
    RUN_TEST(oct_node_create);
    RUN_TEST(oct_tree_add);
    RUN_TEST(oct_tree_remove);
    RUN_TEST(linked_stack);
    RUN_TEST(balance_tree_test);

    GREATEST_MAIN_END();        /* display results */
}