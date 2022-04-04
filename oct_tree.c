//
// Created by cyberpirate on 3/30/22.
//

#include <malloc.h>
#include <memory.h>
#include "oct_tree.h"
#include "linked_stack.h"

uint8_t get_pos_index(const struct Pos* center, struct Pos* pos) {

    uint8_t ret = 0;

    if(center->x <= pos->x) ret += 1;
    ret = ret << 1;

    if(center->y <= pos->y) ret += 1;
    ret = ret << 1;

    if(center->z <= pos->z) ret += 1;

    return ret;
}

struct OctNode* create_oct_node() {
    struct OctNode* ret = (struct OctNode*) malloc(sizeof(struct OctNode));
    memset(ret, 0, sizeof(struct OctNode));
    return ret;
}

void destroy_oct_node(struct OctNode* node) {
    dbgAssert(node->contentType == CT_EMPTY);

    free(node);
}

void explode_node(struct OctNode* node) {

    dbgAssert(node->contentType == CT_LEAVES);

    for(int i = 0; i < NODE_CHILDREN_NUM; i++)
        dbgAssert(node->leaves[i]);

    node->center.x = 0;
    node->center.y = 0;
    node->center.z = 0;

    for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
        node->center.x += node->leaves[i]->pos.x;
        node->center.y += node->leaves[i]->pos.y;
        node->center.z += node->leaves[i]->pos.z;
    }

    node->center.x /= NODE_CHILDREN_NUM;
    node->center.y /= NODE_CHILDREN_NUM;
    node->center.z /= NODE_CHILDREN_NUM;

    node->contentType = CT_NODES;

    struct Leaf* leaves[NODE_CHILDREN_NUM];
    for(int i = 0; i < NODE_CHILDREN_NUM; i++)
        leaves[i] = node->leaves[i];


    for(int i = 0; i < NODE_CHILDREN_NUM; i++)
        node->nodes[i] = create_oct_node();

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
        for(; i < NODE_CHILDREN_NUM; i++) {
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
            node->nodes[get_pos_index(&node->center, &leaf->pos)],
            leaf
        );
    }
}

void remove_leaf(struct OctNode* node, struct Leaf* leaf) {
    dbgAssert(node->contentType != CT_EMPTY);

    if(node->contentType == CT_LEAVES) {

        int i = 0;
        for(; i < NODE_CHILDREN_NUM; i++) {
            if(node->leaves[i] == leaf) {
                node->leaves[i] = nullptr;
                break;
            }
        }

        if(i == NODE_CHILDREN_NUM) {
            dbgAssert(0);
        }

        for(i = 0; i < NODE_CHILDREN_NUM-1; i++) {
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
        remove_leaf(node->nodes[get_pos_index(&node->center, &leaf->pos)], leaf);

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
        for(; i < NODE_CHILDREN_NUM; i++)
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
}

void drain_tree(struct OctNode* node, struct LinkedStackRoot* ls) {

    if(node->contentType == CT_EMPTY) return;

    if(node->contentType == CT_LEAVES) {
        for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
            if(node->leaves[i] != nullptr) {
                add_node(ls, node->leaves[i]);
                node->leaves[i] = nullptr;
            }
        }
        node->contentType = CT_EMPTY;
    }

    if(node->contentType == CT_NODES) {
        for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
            drain_tree(node->nodes[i], ls);
            destroy_oct_node(node->nodes[i]);
            node->nodes[i] = nullptr;
        }
        node->contentType = CT_EMPTY;
    }
}

void bulk_add_leaf(struct OctNode* node, struct LinkedStackRoot* ls) {
    dbgAssert(node->contentType == CT_EMPTY);

    if(ls->count <= NODE_CHILDREN_NUM) {
        while(ls->count > 0) {
            add_leaf(node, get_and_pop_value(ls));
        }
    } else {
        calculate_average_pos(ls, &node->center);

        struct LinkedStackRoot* childLeafStacks[NODE_CHILDREN_NUM];
        for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
            childLeafStacks[i] = create_linked_stack_root();
            node->nodes[i] = create_oct_node();
        }

        while(ls->count > 0) {
            struct Leaf* value = get_and_pop_value(ls);
            add_node(childLeafStacks[get_pos_index(&node->center, &value->pos)], value);
        }

        for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
            bulk_add_leaf(node->nodes[i], childLeafStacks[i]);
            destroy_linked_stack_root(childLeafStacks[i]);
        }

        node->contentType = CT_NODES;
    }
}

void balance_tree(struct OctNode* node) {
    struct LinkedStackRoot* ls = create_linked_stack_root();
    drain_tree(node, ls);
    bulk_add_leaf(node, ls);
    destroy_linked_stack_root(ls);
}