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

/**
 * Calculate center of mass for all internal nodes
 * @param tree
 */
void calc_center_of_mass(struct OctTree* tree);

/**
 * Calculate the force on the leaves
 * @param tree
 */
void calc_force(struct OctTree* tree);

/**
 * Apply calculated force
 * @param tree
 */
void apply_force(struct OctTree* tree);

/**
 * Apply calculated force
 * @param tree
 */
void apply_velocity(struct OctTree* tree);

/**
 * Rebalance tree by finding out of place nodes and moving them
 * @param tree
 */
void rebalance(struct OctTree* tree);

/**
 * Run function on all leaves
 * @param tree
 * @param process_leaf
 */
void walk_leaves(struct OctTree* tree, void (*process_leaf)(struct OctNode* node));

#endif //EC527_PROJECT_OCT_TREE_H
