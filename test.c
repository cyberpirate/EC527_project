//
// Created by cyberpirate on 3/30/22.
//

#include "greatest.h"
#include "oct_tree.h"
#include "rand_gen.h"

//TEST rand_pos_test(void) {
//
//    reset_rand();
//
//    for(int i = 0; i < 5; i++) {
//        Pos pos = rand_pos();
//        printf("x: %.02f\ny: %.02f\nz: %.02f\n", pos.x, pos.y, pos.z);
//    }
//
//    PASS();
//}
//
//TEST oct_node_create(void) {
//
//    struct OctNode* root = create_oct_node();
//
//    ASSERT_EQ(CT_EMPTY, root->contentType);
//
//    destroy_oct_node(root);
//
//    PASS();
//}
//
//TEST oct_tree_add(void) {
//
//    struct OctNode* root = create_oct_node();
//
//    int leafNum = NODE_CHILDREN_NUM*3;
//    struct Leaf* leaves[leafNum];
//
//    for(int i = 0; i < leafNum; i++) {
//        leaves[i] = create_leaf();
//        leaves[i]->pos = rand_pos();
//        if(i == NODE_CHILDREN_NUM) {
//            ASSERT_EQ(CT_LEAVES, root->contentType);
//        }
//        add_leaf(root, leaves[i]);
//        if(i == NODE_CHILDREN_NUM) {
//            ASSERT_EQ(CT_NODES, root->contentType);
//        }
//        ASSERT(leaf_inside(root, leaves[i]));
//    }
//
//    for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
//        for(int j = 0; j < LEAF_CHILDREN_NUM; j++) {
//            if(root->nodes[i]->leaves[j] == nullptr)
//                break;
//            ASSERT(leaf_inside(root->nodes[i], root->nodes[i]->leaves[j]));
//        }
//    }
//
//    ASSERT_EQ(CT_NODES, root->contentType);
//    ASSERT_EQ(leafNum, root->size);
//
//    for(int i = 0; i < leafNum; i++) {
//        remove_leaf(root, leaves[i]);
//        destroy_leaf(leaves[i]);
//    }
//
//    destroy_oct_node(root);
//
//    PASS();
//}
//
//TEST oct_tree_remove(void) {
//
//    struct OctNode* root = create_oct_node();
//
//    int leafNum = (int) (NODE_CHILDREN_NUM * 1.5);
//    struct Leaf* leaves[leafNum];
//
//    for(int i = 0; i < leafNum; i++) {
//        leaves[i] = create_leaf();
//        leaves[i]->pos = rand_pos();
//        add_leaf(root, leaves[i]);
//    }
//
//    for(int i = 0; i < leafNum; i++) {
//        remove_leaf(root, leaves[i]);
//        destroy_leaf(leaves[i]);
//    }
//
//    ASSERT_EQ(0, root->size);
//    ASSERT_EQ(CT_EMPTY, root->contentType);
//
//    destroy_oct_node(root);
//
//    PASS();
//}
//
//TEST oct_tree_calc_barns_hutt(void) {
//
//    struct OctNode* root = create_oct_node();
//
//    int leafNum = NODE_CHILDREN_NUM*3;
//    struct Leaf* leaves[leafNum];
//
//    for(int i = 0; i < leafNum; i++) {
//        leaves[i] = create_leaf();
//        leaves[i]->pos = rand_pos();
//        add_leaf(root, leaves[i]);
//    }
//
//    calc_center_of_mass(root);
//    calc_force(root);
//    apply_force(root);
//    apply_velocity(root);
//    rebalance(root);
//
//    for(int i = 0; i < leafNum; i++) {
//        remove_leaf(root, leaves[i]);
//        destroy_leaf(leaves[i]);
//    }
//
//    destroy_oct_node(root);
//
//    PASS();
//}
//
//TEST oct_node_add_edge(void) {
//
//    struct OctNode* root = create_oct_node();
//
//    struct Leaf* l = create_leaf();
//
//    int xBit = 1 << 2;
//    int yBit = 1 << 1;
//    int zBit = 1 << 0;
//
//    for(int i = 0; i < 8; i++) {
//        l->pos.x = UNIVERSE_SIZE * ((xBit & i) > 0 ? 1 : -1);
//        l->pos.y = UNIVERSE_SIZE * ((yBit & i) > 0 ? 1 : -1);
//        l->pos.z = UNIVERSE_SIZE * ((zBit & i) > 0 ? 1 : -1);
//
//        add_leaf(root, l);
//        remove_leaf(root, l);
//    }
//
//    destroy_leaf(l);
//    destroy_oct_node(root);
//
//    PASS();
//}
//
//TEST oct_node_explode_all_same(void) {
//
//    struct OctNode* root = create_oct_node();
//
//    int leafNum = LEAF_CHILDREN_NUM+1;
//    struct Leaf* l[leafNum];
//
//    for(int i = 0; i < leafNum; i++) {
//        l[i] = create_leaf();
//        l[i]->pos.x = UNIVERSE_SIZE;
//        l[i]->pos.y = UNIVERSE_SIZE;
//        l[i]->pos.z = UNIVERSE_SIZE;
//    }
//
//    for(int i = 0; i < leafNum; i++) {
//        add_leaf(root, l[i]);
//    }
//
//    for(int i = 0; i < leafNum; i++) {
//        remove_leaf(root, l[i]);
//        destroy_leaf(l[i]);
//    }
//
//    destroy_oct_node(root);
//
//    PASS();
//}
//
//TEST oct_node_rebalance_test(void) {
//
//    struct OctNode* root = create_oct_node();
//
//    int leafNum = LEAF_CHILDREN_NUM*3;
//    struct Leaf* l[leafNum];
//
//    for(int i = 0; i < leafNum; i++) {
//        l[i] = create_leaf();
//        l[i]->pos.x = UNIVERSE_SIZE;
//        l[i]->pos.y = UNIVERSE_SIZE;
//        l[i]->pos.z = UNIVERSE_SIZE;
//    }
//
//    for(int i = 0; i < leafNum; i++) {
//        add_leaf(root, l[i]);
//    }
//
//    for(int i = 0; i < leafNum; i++) {
//        l[i]->pos.x = -UNIVERSE_SIZE;
//        l[i]->pos.y = -UNIVERSE_SIZE;
//        l[i]->pos.z = -UNIVERSE_SIZE;
//    }
//
//    rebalance(root);
//
//    for(int i = 0; i < leafNum; i++) {
//        remove_leaf(root, l[i]);
//        destroy_leaf(l[i]);
//    }
//
//    destroy_oct_node(root);
//
//    PASS();
//}

TEST oct_tree_test(void) {
    struct OctTree* tree = create_tree();

    reset_rand();

    int leafNum = 10;

    Pos p;
    for(int i = 0; i < leafNum; i++) {
        p = rand_pos();
        createLeafWithPos(tree, &p);
        if(i > 0)
            ASSERT_EQ(i+1, tree->children[0].node.size);
    }

    ASSERT_EQ(leafNum, tree->children[0].node.size);

    int leafCount = 0;
    for(node_idx_t i = 0; i < tree->elmsCount; i++)
        if(tree->children[i].contentType == CT_LEAF)
            leafCount++;

    ASSERT_EQ(leafNum, leafCount);

    calc_center_of_mass(tree);
    calc_force(tree);
    apply_force(tree);
    apply_velocity(tree);
    rebalance(tree);

    destroy_tree(tree);
    PASS();
}

/* Add all the definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();      /* command-line arguments, initialization. */

    RUN_TEST(oct_tree_test);
//    RUN_TEST(rand_pos_test);
//    RUN_TEST(oct_node_create);
//    RUN_TEST(oct_tree_add);
//    RUN_TEST(oct_tree_remove);
//    RUN_TEST(oct_tree_calc_barns_hutt);
//    RUN_TEST(oct_node_add_edge);
//    RUN_TEST(oct_node_explode_all_same);
//    RUN_TEST(oct_node_rebalance_test);

    GREATEST_MAIN_END();        /* display results */
}