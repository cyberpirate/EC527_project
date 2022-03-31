//
// Created by cyberpirate on 3/30/22.
//

#include "greatest.h"
#include "quad_tree.h"
#include "rand_gen.h"

TEST rand_pos_test(void) {

    reset_rand();

    for(int i = 0; i < 5; i++) {
        struct Pos pos = rand_pos();
        printf("x: %.02f\ny: %.02f\n", pos.x, pos.y);
    }

    PASS();
}

TEST quad_node_create(void) {

    struct Node* root = create_node();

    ASSERT_EQ(CT_EMPTY, root->contentType);

    destroy_node(root);

    PASS();
}

TEST quad_tree_add(void) {

    struct Node* root = create_node();

    for(int i = 0; i < NODE_CHILDREN_NUM+1; i++) {
        struct Leaf* l = create_leaf();
        l->pos = rand_pos();
        add_leaf(root, l);
        if(i < NODE_CHILDREN_NUM) {
            ASSERT_EQ(CT_LEAVES, root->contentType);
        }
    }

    ASSERT_EQ(CT_NODES, root->contentType);
    ASSERT_EQ(NODE_CHILDREN_NUM+1, node_size(root));

    destroy_node(root);

    PASS();
}

TEST quad_tree_remove(void) {

    struct Node* root = create_node();

    int leafNum = 10;
    struct Leaf* leaves[leafNum];

    for(int i = 0; i < leafNum; i++) {
        leaves[i] = create_leaf();
        leaves[i]->pos = rand_pos();
        add_leaf(root, leaves[i]);
    }

    for(int i = 0; i < leafNum; i++) {
        remove_leaf(root, leaves[i]);
    }

    ASSERT_EQ(0, node_size(root));
    ASSERT_EQ(CT_EMPTY, root->contentType);

    destroy_node(root);

    PASS();
}

/* Add all the definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();      /* command-line arguments, initialization. */

    RUN_TEST(rand_pos_test);
    RUN_TEST(quad_node_create);
    RUN_TEST(quad_tree_add);
    RUN_TEST(quad_tree_remove);

    GREATEST_MAIN_END();        /* display results */
}