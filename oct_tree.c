//
// Created by cyberpirate on 3/30/22.
//

#include "oct_tree.h"
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

node_idx_t depth_size(depth_t depth) {
    return pow(8, depth-1);
}

node_idx_t array_size_for_depth(depth_t depth) {
    if(depth == 0) return 0;
    return depth_size(depth) + array_size_for_depth(depth-1);
}

node_idx_t idx_start_for_depth(depth_t depth) {
    return array_size_for_depth(depth);
}

depth_t get_depth_for_idx(node_idx_t idx) {
    depth_t depth = 0;
    node_idx_t start = 0;
    node_idx_t nextIdx = start + depth_size(1);
    while(nextIdx < idx) {
        start = nextIdx;
        nextIdx += depth_size(depth + 2);
        depth++;
    }
    return depth;
}

node_idx_t get_node_children(node_idx_t idx) {
    depth_t depth = get_depth_for_idx(idx);
    node_idx_t depth_start = idx_start_for_depth(depth);
    node_idx_t next_depth_start = idx_start_for_depth(depth+1);
    return next_depth_start + (idx-depth_start)*8;
}

node_idx_t get_node_parent(node_idx_t idx) {
    depth_t depth = get_depth_for_idx(idx);
    node_idx_t pos = (idx - idx_start_for_depth(depth)) / 8;
    return get_depth_for_idx(depth-1) + pos*8;
}

uint8_t get_pos_index(Pos* center, Pos* pos) {

    uint8_t ret = 0;

    if(center->x <= pos->x) ret += 1;
    ret = ret << 1;

    if(center->y <= pos->y) ret += 1;
    ret = ret << 1;

    if(center->z <= pos->z) ret += 1;

    return ret;
}

void update_extents(Pos* minExt, Pos* maxExt, uint8_t pos_index) {
    int xBit = 1 << 2;
    int yBit = 1 << 1;
    int zBit = 1;

    Pos center;
    calc_center(minExt, maxExt, &center);

    if((pos_index & xBit) > 0) {
        minExt->x = center.x;
    } else {
        maxExt->x = center.x;
    }

    if((pos_index & yBit) > 0) {
        minExt->y = center.y;
    } else {
        maxExt->y = center.y;
    }

    if((pos_index & zBit) > 0) {
        minExt->z = center.z;
    } else {
        maxExt->z = center.z;
    }
}

void set_tree_depth(struct OctTree* tree, depth_t depth) {
    uint32_t oldSize = tree->elmsCount * sizeof(struct OctNode);
    tree->elmsCount = array_size_for_depth(depth);
    uint32_t size = tree->elmsCount * sizeof(struct OctNode);
    tree->children = realloc(tree->children, size);
    tree->depth = depth;
    if(oldSize < size) {
        memset(((void*) tree->children) + oldSize, 0, size-oldSize);
    }
}

struct OctNode* getNode(struct OctTree* tree, node_idx_t idx) {
    return &tree->children[idx];
}

void setNodeToLeaf(struct OctNode* node, struct Leaf* leaf) {
    node->leaf = *leaf;
    node->contentType = CT_LEAF;
}

void setNodeEmpty(struct OctNode* node) {
    memset(node, 0, sizeof(struct OctNode));
}

void setNodeINode(struct OctTree* tree, node_idx_t idx, Pos* center) {
    struct OctNode* node = getNode(tree, idx);
    struct Leaf leaf = node->leaf;
    setNodeEmpty(node);
    node->contentType = CT_INODE;
    node->node.size = 1;
    node_idx_t childIdx = get_node_children(idx) + get_pos_index(center, &leaf.pos);
    setNodeToLeaf(getNode(tree, childIdx), &leaf);
}

//// PUBLIC SECTION ////

struct OctTree* create_tree() {
    struct OctTree* tree = (struct OctTree*) malloc(sizeof(struct OctTree));
    tree->children = tree->children = malloc(sizeof(struct OctNode));
    tree->depth = 1;
    tree->elmsCount = 1;
    memset(tree->children, 0, sizeof(struct OctNode));
    tree->universeSize.x = UNIVERSE_SIZE;
    tree->universeSize.y = UNIVERSE_SIZE;
    tree->universeSize.z = UNIVERSE_SIZE;
    return tree;
}

void destroy_tree(struct OctTree* tree) {
    free(tree->children);
    free(tree);
}

void addLeaf(struct OctTree* tree, struct Leaf* leaf) {
    node_idx_t idx = 0;
    depth_t depth = 0;

    Pos minExt = tree->universeSize;
    Pos maxExt = tree->universeSize;
    mult_scalar(&minExt, -1);

    Pos center;

    calc_center(&minExt, &maxExt, &center);

    while(true) {
        struct OctNode* node = getNode(tree, idx);
        if(node->contentType == CT_EMPTY) {
            setNodeToLeaf(node, leaf);
            while(idx > 0) {
                idx = get_node_parent(idx);
                getNode(tree, idx)->node.size++;
            }
            break;
        } else {
            if(node->contentType == CT_LEAF) {
                if(tree->depth <= depth+1) {
                    set_tree_depth(tree, depth+2);
                }
                setNodeINode(tree, idx, &center);
            }

            uint8_t pos_index = get_pos_index(&center, &leaf->pos);
            idx = get_node_children(idx) + pos_index;
            update_extents(&minExt, &maxExt, pos_index);
            calc_center(&minExt, &maxExt, &center);
            depth++;
        }
    }
}

void createLeafWithPos(struct OctTree* tree, Pos* pos) {
    struct Leaf leaf;
    leaf.pos = *pos;
    addLeaf(tree, &leaf);
}