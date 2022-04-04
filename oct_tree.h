//
// Created by cyberpirate on 3/30/22.
//

#ifndef EC527_PROJECT_OCT_TREE_H
#define EC527_PROJECT_OCT_TREE_H

#include <stdint-gcc.h>
#include "params.h"

#define CT_EMPTY  0
#define CT_LEAVES 1
#define CT_NODES  2

#define NODE_CHILDREN_NUM 8

struct LinkedStackRoot;

struct Pos {
    coord_t x;
    coord_t y;
    coord_t z;
};

struct Leaf {
    struct Pos pos;
};

struct OctNode {
    uint8_t contentType;
    struct Pos center;
    union {
        struct Leaf* leaves[NODE_CHILDREN_NUM];
        struct OctNode* nodes[NODE_CHILDREN_NUM];
    };
};

/**
 * Find the octant of pos relative to center
 * @param center
 * @param pos
 * @return
 */
uint8_t get_pos_index(const struct Pos* center, struct Pos* pos);

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
 * Return the size of a node
 * @param node
 * @return
 */
int node_size(struct OctNode* node);

/**
 * Drain all leaves to linked stack
 * @param node
 */
void drain_tree(struct OctNode* node, struct LinkedStackRoot* ls);

/**
 * Bulk add leaves in a balanced way.
 * This should only be used on an empty tree.
 * @param node
 * @param ls
 */
void bulk_add_leaf(struct OctNode* node, struct LinkedStackRoot* ls);

/**
 * Balance a tree
 * @param node
 */
void balance_tree(struct OctNode* node);

#endif //EC527_PROJECT_OCT_TREE_H
