//
// Created by cyberpirate on 3/30/22.
//

#include <malloc.h>
#include <assert.h>
#include <memory.h>
#include "quad_tree.h"

uint8_t get_pos_index(const struct Pos* center, const struct Pos* pos) {

    uint8_t ret = 0;

    if(center->x <= pos->x) ret += 1;
    ret = ret << 1;

    if(center->y <= pos->y) ret += 1;
    ret = ret << 1;

    if(center->z <= pos->z) ret += 1;

    return ret;
}

struct Node* create_node() {
    struct Node* ret = (struct Node*) malloc(sizeof(struct Node));
    memset(ret, 0, sizeof(struct Node));
    return ret;
}

void destroy_node(struct Node* node) {

    if(node->contentType == CT_LEAVES) {
        for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
            destroy_leaf(node->leaves[i]);
        }
    }

    if(node->contentType == CT_NODES) {
        for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
            destroy_node(node->nodes[i]);
        }
    }

    free(node);
}

void explode_node(struct Node* node) {

    assert(node->contentType == CT_LEAVES);

    for(int i = 0; i < NODE_CHILDREN_NUM; i++)
        assert(node->leaves[i]);

    node->center.x = 0;
    node->center.y = 0;

    for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
        node->center.x += node->leaves[i]->pos.x;
        node->center.y += node->leaves[i]->pos.y;
    }

    node->center.x /= NODE_CHILDREN_NUM;
    node->center.y /= NODE_CHILDREN_NUM;

    node->contentType = CT_NODES;

    struct Leaf* leaves[NODE_CHILDREN_NUM];
    for(int i = 0; i < NODE_CHILDREN_NUM; i++)
        leaves[i] = node->leaves[i];


    for(int i = 0; i < NODE_CHILDREN_NUM; i++)
        node->nodes[i] = create_node();

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

void add_leaf(struct Node* node, struct Leaf* leaf) {
    if(node->contentType == CT_EMPTY) {
        node->contentType = CT_LEAVES;
    }

    if(node->contentType == CT_LEAVES) {
        int i = 0;
        for(; i < NODE_CHILDREN_NUM; i++) {
            if(node->leaves[i] == 0) {
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
            node->nodes[get_pos_index(&node->center, &leaf->pos)],
            leaf
        );
    }
}

void remove_leaf(struct Node* node, struct Leaf* leaf) {
    assert(node->contentType != CT_EMPTY);

    if(node->contentType == CT_LEAVES) {

        int i = 0;
        for(; i < NODE_CHILDREN_NUM; i++) {
            if(node->leaves[i] == leaf) {
                node->leaves[i] = 0;
                break;
            }
        }

        if(i == NODE_CHILDREN_NUM) {
            assert(0);
        }

        for(i = 0; i < NODE_CHILDREN_NUM-1; i++) {
            if(node->leaves[i] == 0) {
                node->leaves[i] = node->leaves[i+1];
                node->leaves[i+1] = 0;
            }
        }

        if(node->leaves[0] == 0) {
            node->contentType = CT_EMPTY;
        }
    }

    if(node->contentType == CT_NODES) {
        remove_leaf(node->nodes[get_pos_index(&node->center, &leaf->pos)], leaf);

        int i = 0;
        for(; i < NODE_CHILDREN_NUM; i++) {
            if(node->nodes[i]->contentType != CT_EMPTY) {
                break;
            }
        }

        if(i == NODE_CHILDREN_NUM) {
            for(; i < NODE_CHILDREN_NUM; i++) {
                destroy_node(node->nodes[i]);
            }
            node->contentType = CT_EMPTY;
        }
    }
}

int node_size(struct Node* node) {
    if(node->contentType == CT_EMPTY) {
        return 0;
    }

    if(node->contentType == CT_LEAVES) {
        int i = 0;
        for(; i < NODE_CHILDREN_NUM; i++)
            if(node->leaves[i] == 0)
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

    assert(0);
}

