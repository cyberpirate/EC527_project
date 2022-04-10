//
// Created by cyberpirate on 3/30/22.
//

#ifndef EC527_PROJECT_OCT_TREE_H
#define EC527_PROJECT_OCT_TREE_H

#include <stdint-gcc.h>
#include "params.h"
#include "utils.h"

#define CT_EMPTY  0
#define CT_LEAVES 1
#define CT_NODES  2

#define NODE_CHILDREN_NUM 8
#define LEAF_CHILDREN_NUM 8

struct Leaf {
    Pos pos;
    Force force;
    Velocity velocity;
};

struct OctNode {
    uint8_t contentType;
    uint16_t size;
    struct OctNode* parent;
    Pos maxExt;
    Pos minExt;
    Pos centerOfMass;
    union {
        struct Leaf* leaves[LEAF_CHILDREN_NUM];
        struct OctNode* nodes[NODE_CHILDREN_NUM];
    };
};

/**
 * Find the octant of pos relative to center.
 * The return is three least significant bits, set to 1 if positive
 * as follows
 * +x | +y | +z
 * @param minExt
 * @param maxExt
 * @param pos
 * @return
 */
uint8_t get_pos_index(Pos* minExt, Pos* maxExt, Pos* pos);

/**
 * Create an empty node
 * @return
 */
struct OctNode* create_oct_node();

/**
 * Release node and its children
 * @param node
 */
void destroy_oct_node(struct OctNode* node);

/**
 * Explode from leaves into nodes
 * @param node
 */
void explode_node(struct OctNode* node);

/**
 * Create an empty leaf
 * @return
 */
struct Leaf* create_leaf();

/**
 * Release leaf
 * @param leaf
 */
void destroy_leaf(struct Leaf* leaf);

/**
 * Add a leaf to the node or its children
 * @param node
 * @param leaf
 */
void add_leaf(struct OctNode* node, struct Leaf* leaf);

/**
 * Remove a leaf from a node
 * @param node
 * @param leaf
 */
void remove_leaf(struct OctNode* node, struct Leaf* leaf);

/**
 * Check if leaf is inside node extents
 * @param node
 * @param leaf
 * @return
 */
uint8_t leaf_inside(struct OctNode* node, struct Leaf* leaf);

/**
 * Calculate the center of mass for a node
 * @param node
 */
void calc_center_of_mass(struct OctNode* node);

/**
 * Calculate the forces for all leaves in the tree
 * @param node
 */
void calc_force(struct OctNode* node);

/**
 * Apply force calculated across all leaves
 * @param node
 */
void apply_force(struct OctNode* node);

/**
 * Apply velocity calculated across all leaves
 * @param node
 */
void apply_velocity(struct OctNode* node);

/**
 * Check if any leaves are out of place and fix them
 * @param node
 */
void rebalance(struct OctNode* node);

#endif //EC527_PROJECT_OCT_TREE_H
