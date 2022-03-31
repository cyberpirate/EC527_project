//
// Created by cyberpirate on 3/30/22.
//

#ifndef EC527_PROJECT_QUAD_TREE_H
#define EC527_PROJECT_QUAD_TREE_H

#include <stdint-gcc.h>
#include "params.h"

#define CT_EMPTY  0
#define CT_LEAVES 1
#define CT_NODES  2

#define QUADRANT_TR 0
#define QUADRANT_TL 1
#define QUADRANT_BL 2
#define QUADRANT_BR 3

#define NODE_CHILDREN_NUM 4

struct Pos {
    coord_t x;
    coord_t y;
};

struct Leaf {
    struct Pos pos;
};

struct Node {
    uint8_t contentType;
    struct Pos center;
    union {
        struct Leaf* leaves[NODE_CHILDREN_NUM];
        struct Node* nodes[NODE_CHILDREN_NUM];
    };
};

/**
 * Find the quadrant of pos relative to center
 * @param center
 * @param pos
 * @return
 */
uint8_t get_quadrant(const struct Pos* center, const struct Pos* pos);

/**
 * Create an empty node
 * @return
 */
struct Node* create_node();

/**
 * Release node and its children
 * @param node
 */
void destroy_node(struct Node* node);

/**
 * Explode from leaves into nodes
 * @param node
 */
void explode_node(struct Node* node);

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
void add_leaf(struct Node* node, struct Leaf* leaf);

/**
 * Remove a leaf from a node
 * @param node
 * @param leaf
 */
void remove_leaf(struct Node* node, struct Leaf* leaf);

/**
 * Return the size of a node
 * @param node
 * @return
 */
int node_size(struct Node* node);

#endif //EC527_PROJECT_QUAD_TREE_H
