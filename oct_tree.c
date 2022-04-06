//
// Created by cyberpirate on 3/30/22.
//

#include <malloc.h>
#include <memory.h>
#include "oct_tree.h"
#include "linked_stack.h"

uint8_t get_pos_index(struct Pos* minExt, struct Pos* maxExt, struct Pos* pos) {

    uint8_t ret = 0;
    struct Pos center;

    center.x = (maxExt->x + minExt->x) / 2;
    center.y = (maxExt->y + minExt->y) / 2;
    center.z = (maxExt->z + minExt->z) / 2;

    if(center.x <= pos->x) ret += 1;
    ret = ret << 1;

    if(center.y <= pos->y) ret += 1;
    ret = ret << 1;

    if(center.z <= pos->z) ret += 1;

    return ret;
}

struct OctNode* create_oct_node() {
    struct OctNode* ret = (struct OctNode*) malloc(sizeof(struct OctNode));
    memset(ret, 0, sizeof(struct OctNode));

    ret->maxExt.x = UNIVERSE_SIZE;
    ret->maxExt.y = UNIVERSE_SIZE;
    ret->maxExt.z = UNIVERSE_SIZE;

    ret->minExt.x = -UNIVERSE_SIZE;
    ret->minExt.y = -UNIVERSE_SIZE;
    ret->minExt.z = -UNIVERSE_SIZE;

    return ret;
}

void destroy_oct_node(struct OctNode* node) {
    dbgAssert(node->contentType == CT_EMPTY);

    free(node);
}

void explode_node(struct OctNode* node) {

    dbgAssert(node->contentType == CT_LEAVES);

    struct Pos center;

    center.x = (node->maxExt.x + node->minExt.x) / 2;
    center.y = (node->maxExt.y + node->minExt.y) / 2;
    center.z = (node->maxExt.z + node->minExt.z) / 2;

    struct Leaf* leaves[NODE_CHILDREN_NUM];
    for(int i = 0; i < NODE_CHILDREN_NUM; i++)
        leaves[i] = node->leaves[i];

    node->contentType = CT_NODES;

    int xBit = 1 << 2;
    int yBit = 1 << 1;
    int zBit = 1;

    for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
        node->nodes[i] = create_oct_node();
        node->nodes[i]->minExt = node->minExt;
        node->nodes[i]->maxExt = node->maxExt;

        if((i & xBit) > 0) {
            node->nodes[i]->minExt.x = center.x;
        } else {
            node->nodes[i]->maxExt.x = center.x;
        }

        if((i & yBit) > 0) {
            node->nodes[i]->minExt.y = center.y;
        } else {
            node->nodes[i]->maxExt.y = center.y;
        }

        if((i & zBit) > 0) {
            node->nodes[i]->minExt.z = center.z;
        } else {
            node->nodes[i]->maxExt.z = center.z;
        }
    }

    for(int i = 0; i < NODE_CHILDREN_NUM; i++)
        add_leaf(node, leaves[i]);
}

struct Leaf* create_leaf() {
    struct Leaf* ret = (struct Leaf*) malloc(sizeof(struct Leaf));
    memset(ret, 0, sizeof(struct Leaf));
    return ret;
}

void destroy_leaf(struct Leaf* leaf) {
    free(leaf);
}

void add_leaf(struct OctNode* node, struct Leaf* leaf) {
    if(node->contentType == CT_EMPTY) {
        node->contentType = CT_LEAVES;
    }

    if(node->contentType == CT_LEAVES) {
        int i = 0;
        for(; i < LEAF_CHILDREN_NUM; i++) {
            if(node->leaves[i] == nullptr) {
                node->leaves[i] = leaf;
                break;
            }
        }

        if(i == NODE_CHILDREN_NUM) {
            explode_node(node);
        }
    }

    if(node->contentType == CT_NODES) {
        add_leaf(
            node->nodes[get_pos_index(&node->minExt, &node->maxExt, &leaf->pos)],
            leaf
        );
    }
}

void remove_leaf(struct OctNode* node, struct Leaf* leaf) {
    dbgAssert(node->contentType != CT_EMPTY);

    if(node->contentType == CT_LEAVES) {

        int i = 0;
        for(; i < LEAF_CHILDREN_NUM; i++) {
            if(node->leaves[i] == leaf) {
                node->leaves[i] = nullptr;
                break;
            }
        }

        if(i == LEAF_CHILDREN_NUM) {
            dbgAssert(0);
        }

        for(i = 0; i < LEAF_CHILDREN_NUM-1; i++) {
            if(node->leaves[i] == nullptr) {
                node->leaves[i] = node->leaves[i+1];
                node->leaves[i+1] = nullptr;
            }
        }

        if(node->leaves[0] == nullptr) {
            node->contentType = CT_EMPTY;
        }
    }

    if(node->contentType == CT_NODES) {
        remove_leaf(node->nodes[get_pos_index(&node->minExt, &node->maxExt, &leaf->pos)], leaf);

        int i = 0;
        for(; i < NODE_CHILDREN_NUM; i++) {
            if(node->nodes[i]->contentType != CT_EMPTY) {
                break;
            }
        }

        if(i == NODE_CHILDREN_NUM) {
            for(i = 0; i < NODE_CHILDREN_NUM; i++) {
                destroy_oct_node(node->nodes[i]);
                node->nodes[i] = nullptr;
            }
            node->contentType = CT_EMPTY;
        }
    }
}

int node_size(struct OctNode* node) {
    if(node->contentType == CT_EMPTY) {
        return 0;
    }

    if(node->contentType == CT_LEAVES) {
        int i = 0;
        for(; i < LEAF_CHILDREN_NUM; i++)
            if(node->leaves[i] == nullptr)
                break;
        return i;
    }

    if(node->contentType == CT_NODES) {
        int c = 0;
        for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
            c += node_size(node->nodes[i]);
        }
        return c;
    }

    dbgAssert(0);
    return 0;
}

uint8_t leaf_inside(struct OctNode* node, struct Leaf* leaf) {
    return
        node->minExt.x <= leaf->pos.x && leaf->pos.x <= node->maxExt.x &&
        node->minExt.y <= leaf->pos.y && leaf->pos.y <= node->maxExt.y &&
        node->minExt.z <= leaf->pos.z && leaf->pos.z <= node->maxExt.z;
}