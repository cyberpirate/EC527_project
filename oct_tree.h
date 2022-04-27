//
// Created by cyberpirate on 3/30/22.
//

#ifndef EC527_PROJECT_OCT_TREE_H
#define EC527_PROJECT_OCT_TREE_H

#include <stdint-gcc.h>
#include "params.h"
#include "utils.h"
#include <stdbool.h>

#define CT_EMPTY  0
#define CT_LEAVES 1
#define CT_NODES  2

#define NODE_CHILD_COUNT 8
#define LEAF_CHILD_COUNT 10

typedef uint8_t depth_t;
typedef uint32_t node_idx_t;
typedef uint32_t leaf_idx_t;

struct Leaf {
    Pos pos;
    Force force;
    Velocity velocity;
};

struct OctNode {
    uint8_t contentType;
    node_idx_t size;
    Pos centerOfMass;
    leaf_idx_t leaves[LEAF_CHILD_COUNT];
};

struct OctTree {
    struct Leaf* leaves;
    leaf_idx_t leaf_count;

//    struct OctNode* children;
//    node_idx_t elmsCount;

    struct OctNode** depth;
    depth_t depth_count;
};

/**
 * Create empty tree
 * @return
 */
struct OctTree* create_tree(leaf_idx_t leaf_count);

/**
 * Destroy tree
 * @param tree
 */
void destroy_tree(struct OctTree* tree);

/**
 * Add leaves to tree after initialization
 */
void add_leaves_to_tree(struct OctTree* tree);

/**
 * Add a leaf to the tree
 * @param tree
 * @param leaf
 */
void addLeaf(struct OctTree* tree, leaf_idx_t leaf_idx);

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
void walk_leaves(struct OctTree* tree, void (*process_leaf)(struct OctTree* tree, struct Leaf* leaf));

//region private functions

struct Extents {
    Pos minExt;
    Pos maxExt;
    Pos center;
};

typedef uint8_t child_pos_idx_t;

struct Leaf* getLeaf(struct OctTree* tree, node_idx_t node_idx, child_pos_idx_t leaf_idx);
struct OctNode* getNode(struct OctTree* tree, node_idx_t node_idx);
node_idx_t depth_size(depth_t depth);
node_idx_t array_size_for_depth(depth_t depth);
node_idx_t idx_start_for_depth(depth_t depth);
depth_t get_depth_for_idx(node_idx_t idx);
node_idx_t get_node_children(node_idx_t idx);
node_idx_t get_node_parent(node_idx_t idx);
void set_tree_depth(struct OctTree* tree, depth_t depth_count);
void setNodeEmpty(struct OctNode* node);
void setNodeToLeafNode(struct OctNode* node);
void addLeafToLeafNode(struct OctNode* node, leaf_idx_t idx);
void removeLeafFromNode(struct OctNode* node, child_pos_idx_t idx);
bool setNodeToInternalNode(struct OctTree* tree, node_idx_t idx, struct Extents* ext);
struct Extents get_max_extents();
bool pos_inside(struct Extents* ext, Pos* pos);
void update_extents(struct Extents* ext, child_pos_idx_t pos_index);
child_pos_idx_t get_pos_index(struct Extents* ext, Pos* pos);
bool addLeafToNode(struct OctTree* tree, leaf_idx_t leaf_idx, node_idx_t idx, struct Extents* ext);
Pos rand_inside(struct Extents* ext);
void addLeafToSmallestNode(struct OctTree* tree, leaf_idx_t leaf_idx, node_idx_t idx, struct Extents* ext);
void calc_node_center_of_mass(struct OctTree* tree, node_idx_t idx);
bool calc_force_for_node(struct OctTree* tree, node_idx_t idx, struct Extents* ext, void* callback_arg);
void calc_force_on_leaf(struct OctTree* tree, struct Leaf* leaf);
void apply_force_on_leaf(struct OctTree* tree, struct Leaf* leaf);
void apply_velocity_on_leaf(struct OctTree* tree, struct Leaf* leaf);
bool rebalance_node(struct OctTree* tree, node_idx_t idx, struct Extents* ext, void* callback_arg);


//endregion private functions

#endif //EC527_PROJECT_OCT_TREE_H
