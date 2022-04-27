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

TEST node_util_function_tests(void) {

    ASSERT_EQ(1, depth_size(0));
    ASSERT_EQ(8, depth_size(1));
    ASSERT_EQ(8*8, depth_size(2));
    ASSERT_EQ(8*8*8, depth_size(3));

    ASSERT_EQ(1, array_size_for_depth(0));
    ASSERT_EQ(8 + 1, array_size_for_depth(1));
    ASSERT_EQ(8*8 + 8 + 1, array_size_for_depth(2));
    ASSERT_EQ(8*8*8 + 8*8 + 8 + 1, array_size_for_depth(3));

    ASSERT_EQ(0, idx_start_for_depth(0));
    ASSERT_EQ(1, idx_start_for_depth(1));
    ASSERT_EQ(8 + 1, idx_start_for_depth(2));
    ASSERT_EQ(8*8 + 8 + 1, idx_start_for_depth(3));

    ASSERT_EQ(0, get_depth_for_idx(0));
    ASSERT_EQ(1, get_depth_for_idx(1));
    ASSERT_EQ(2, get_depth_for_idx(8 + 1));
    ASSERT_EQ(3, get_depth_for_idx(8*8 + 8 + 1));

    for(depth_t i = 0; i < 10; i++) {
        ASSERT_EQ(i, get_depth_for_idx(idx_start_for_depth(i)));
    }

    ASSERT_EQ(1, get_node_children(0));
    ASSERT_EQ(9, get_node_children(1));
    ASSERT_EQ(17, get_node_children(2));

    ASSERT_EQ(0, get_node_parent(1));
    ASSERT_EQ(0, get_node_parent(8));
    ASSERT_EQ(1, get_node_parent(9));
    ASSERT_EQ(1, get_node_parent(16));
    ASSERT_EQ(2, get_node_parent(17));
    ASSERT_EQ(2, get_node_parent(24));

    for(node_idx_t i = 0; i < 10; i++) {
        for(node_idx_t child = 0; child < NODE_CHILD_COUNT; child++) {
            ASSERT_EQ(i, get_node_parent(get_node_children(i) + child));
        }
    }

    PASS();
}

TEST oct_tree_test(void) {
    node_idx_t leafNum = 10;
    struct OctTree* tree = create_tree(leafNum);

    reset_rand();

    for(leaf_idx_t i = 0; i < tree->leaf_count; i++) {
        tree->leaves[i].pos = rand_pos();
    }
    for(leaf_idx_t i = 0; i < tree->leaf_count; i++) {
        addLeaf(tree, i);
        ASSERT_EQ(i+1, getNode(tree, 0)->size);
    }

    node_idx_t leafCount = 0;
    for(node_idx_t i = 0; i < array_size_for_depth(tree->depth_count-1); i++)
        if(getNode(tree, i)->contentType == CT_LEAVES)
            leafCount += getNode(tree, i)->size;

    ASSERT_EQ(leafNum, leafCount);

    calc_center_of_mass(tree);
    calc_force(tree);
    apply_force(tree);
    apply_velocity(tree);
    rebalance(tree);

    destroy_tree(tree);
    PASS();
}

TEST scatter_test(void) {
    node_idx_t leafNum = LEAF_CHILD_COUNT*20;
    struct OctTree* tree = create_tree(leafNum);

    reset_rand();

    for(leaf_idx_t i = 0; i < tree->leaf_count; i++) {
        tree->leaves[i].pos.x = 0;
        tree->leaves[i].pos.y = 0;
        tree->leaves[i].pos.z = 0;
    }

    add_leaves_to_tree(tree);

    destroy_tree(tree);
    PASS();
}

/* Add all the definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();      /* command-line arguments, initialization. */

//    RUN_TEST(node_util_function_tests);
//    RUN_TEST(oct_tree_test);
    RUN_TEST(scatter_test);

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