//
// Created by cyberpirate on 3/30/22.
//

#ifndef EC527_PROJECT_OCT_TREE_H
#define EC527_PROJECT_OCT_TREE_H

#include <stdint-gcc.h>
#include "params.h"
#include "utils.h"

#define CT_EMPTY  0
#define CT_LEAF   1
#define CT_INODE  2

#define NODE_CHILDREN_NUM 8
#define LEAF_CHILDREN_NUM 8

typedef uint8_t depth_t;
typedef uint32_t node_idx_t;

struct Leaf {
    Pos pos;
    Force force;
    Velocity velocity;
};

struct HiddenNode {
    Pos centerOfMass;
    node_idx_t size;
};

//struct OctNode {
//    uint8_t contentType;
//    uint16_t size;
//    struct OctNode* parent;
//    Pos maxExt;
//    Pos minExt;
//    Pos centerOfMass;
//    union {
//        struct Leaf* leaves[LEAF_CHILDREN_NUM];
//        struct OctNode* nodes[NODE_CHILDREN_NUM];
//    };
//};

struct OctNode {
    uint8_t contentType;
    union {
        struct Leaf leaf;
        struct HiddenNode node;
    };
};

struct OctTree {
    struct OctNode* children;
    node_idx_t elmsCount;
    depth_t depth;
    Pos universeSize;
};

/**
 * Create empty tree
 * @return
 */
struct OctTree* create_tree();

/**
 * Destroy tree
 * @param tree
 */
void destroy_tree(struct OctTree* tree);

/**
 * get node for index
 * @param idx
 * @return
 */
struct OctNode* getNode(struct OctTree* tree, node_idx_t idx);

/**
 * Add a leaf to the tree
 * @param tree
 * @param leaf
 */
void addLeaf(struct OctTree* tree, struct Leaf* leaf);

/**
 * Initialize a leaf with a pos
 *
 * @param tree
 * @param leaf
 */
void createLeafWithPos(struct OctTree* tree, Pos* pos);

#endif //EC527_PROJECT_OCT_TREE_H
