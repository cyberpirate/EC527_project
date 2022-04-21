//
// Created by cyberpirate on 3/30/22.
//

#include "oct_tree.h"
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

struct Extents {
    Pos minExt;
    Pos maxExt;
    Pos center;
};

typedef uint8_t child_pos_idx_t;

struct Extents get_max_extents() {
    struct Extents ret;
    ret.maxExt.x =  UNIVERSE_SIZE;
    ret.maxExt.y =  UNIVERSE_SIZE;
    ret.maxExt.z =  UNIVERSE_SIZE;
    ret.minExt.x = -UNIVERSE_SIZE;
    ret.minExt.y = -UNIVERSE_SIZE;
    ret.minExt.z = -UNIVERSE_SIZE;
    ret.center.x =  0;
    ret.center.y =  0;
    ret.center.z =  0;
    return ret;
}

bool pos_inside(struct Extents* ext, Pos* pos) {
    return (
        (ext->minExt.x <= pos->x && pos->x < ext->maxExt.x) &&
        (ext->minExt.y <= pos->y && pos->y < ext->maxExt.y) &&
        (ext->minExt.z <= pos->z && pos->z < ext->maxExt.z)
    );
}

void update_extents(struct Extents* ext, child_pos_idx_t pos_index) {
    int xBit = 1 << 2;
    int yBit = 1 << 1;
    int zBit = 1;

    if((pos_index & xBit) > 0) {
        ext->minExt.x = ext->center.x;
    } else {
        ext->maxExt.x = ext->center.x;
    }

    if((pos_index & yBit) > 0) {
        ext->minExt.y = ext->center.y;
    } else {
        ext->maxExt.y = ext->center.y;
    }

    if((pos_index & zBit) > 0) {
        ext->minExt.z = ext->center.z;
    } else {
        ext->maxExt.z = ext->center.z;
    }

    ext->center.x = (ext->minExt.x + ext->maxExt.x) / 2;
    ext->center.y = (ext->minExt.y + ext->maxExt.y) / 2;
    ext->center.z = (ext->minExt.z + ext->maxExt.z) / 2;
}

child_pos_idx_t get_pos_index(struct Extents* ext, Pos* pos) {

    child_pos_idx_t ret = 0;

    if(ext->center.x <= pos->x) ret += 1;
    ret = ret << 1;

    if(ext->center.y <= pos->y) ret += 1;
    ret = ret << 1;

    if(ext->center.z <= pos->z) ret += 1;

    return ret;
}

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
    if(tree->elmsCount <= idx) return nullptr;
    return &tree->children[idx];
}

void setNodeToLeaf(struct OctNode* node, struct Leaf* leaf) {
    node->leaf = *leaf;
    node->contentType = CT_LEAF;
}

void setNodeEmpty(struct OctNode* node) {
    memset(node, 0, sizeof(struct OctNode));
}

void setNodeINode(struct OctTree* tree, node_idx_t idx, struct Extents* ext) {
    struct OctNode* node = getNode(tree, idx);
    struct Leaf leaf = node->leaf;
    setNodeEmpty(node);
    node->contentType = CT_INODE;
    node->node.size = 1;
    node_idx_t childIdx = get_node_children(idx) + get_pos_index(ext, &leaf.pos);
    setNodeToLeaf(getNode(tree, childIdx), &leaf);
}

void walk_tree(struct OctTree* tree, node_idx_t idx, struct Extents* ext, bool (*process_callback)(struct OctTree* tree, node_idx_t idx, struct Extents* ext, void* callbackArg), void* callback_arg) {
    struct OctNode* node = getNode(tree, idx);

    if(node == nullptr) return;
    if(node->contentType == CT_EMPTY) return;

    if(!process_callback(tree, idx, ext, callback_arg)) return;

    if(node->contentType == CT_INODE) {
        node_idx_t children_idx = get_node_children(idx);

        for(int i = 0; i < 8; i++) {
            node_idx_t child_idx = children_idx + i;
            struct Extents childExt = *ext;
            update_extents(&childExt, child_idx);
            walk_tree(tree, child_idx, &childExt, process_callback, callback_arg);
        }
    }
}

void calc_node_center_of_mass(struct OctTree* tree, node_idx_t idx) {
    struct OctNode* node = getNode(tree, idx);

    if(node == nullptr) return;
    if(node->contentType == CT_EMPTY) return;
    if(node->contentType == CT_LEAF) return;

    node_idx_t children_idx = get_node_children(idx);

    node->node.centerOfMass.x = 0;
    node->node.centerOfMass.y = 0;
    node->node.centerOfMass.z = 0;

    for(int i = 0; i < 8; i++) {
        node_idx_t child_idx = children_idx + i;
        calc_node_center_of_mass(tree, child_idx);

        struct OctNode* childNode = getNode(tree, child_idx);
        if(childNode == nullptr) continue;
        if(childNode->contentType == CT_EMPTY) continue;

        Pos* childPos = childNode->contentType == CT_LEAF ? &childNode->leaf.pos : &childNode->node.centerOfMass;
        node_idx_t size = childNode->contentType == CT_LEAF ? 1 : childNode->node.size;

        node->node.centerOfMass.x += childPos->x * size;
        node->node.centerOfMass.y += childPos->y * size;
        node->node.centerOfMass.z += childPos->z * size;
    }

    node->node.centerOfMass.x /= node->node.size;
    node->node.centerOfMass.y /= node->node.size;
    node->node.centerOfMass.z /= node->node.size;
}

bool calc_force_for_node(struct OctTree* tree, node_idx_t idx, struct Extents* ext, void* callback_arg) {
    struct OctNode* dstNode = (struct OctNode*) callback_arg;
    struct OctNode* node = getNode(tree, idx);

    if(dstNode == node) return true;

    if(node->contentType == CT_LEAF) {
        coord_t d = dist(&dstNode->leaf.pos, &node->leaf.pos);
        if(d == 0) return true;
        coord_t f = (G*2)/(d*d);
        Force fVec = vec_dir(&dstNode->leaf.pos, &node->leaf.pos);
        mult_scalar(&fVec, f);

        add(&dstNode->leaf.force, &fVec);
    }

    if(node->contentType == CT_INODE) {
        coord_t s = ext->maxExt.x - ext->minExt.x;
        s = s > 0 ? s : -s;

        coord_t d = dist(&node->node.centerOfMass, &dstNode->leaf.pos);

        if(s/d < SD_THRESHOLD) { // node is far enough away

            coord_t f = (G*(node->node.size + 1))/(d*d);
            Force fVec = vec_dir(&dstNode->leaf.pos, &node->node.centerOfMass);
            mult_scalar(&fVec, f);

            add(&dstNode->leaf.force, &fVec);

            return false;
        } else { // node is too close
            return true;
        }
    }

    return true;
}

bool calc_force_on_node(struct OctTree* tree, node_idx_t idx, struct Extents* ext, void* callback_arg) {
    struct OctNode* node = getNode(tree, idx);
    if(node->contentType == CT_INODE) return true;

    struct Extents rootExt = get_max_extents();
    walk_tree(tree, 0, &rootExt, calc_force_for_node, node);

    return false;
}

bool apply_force_on_node(struct OctTree* tree, node_idx_t idx, struct Extents* ext, void* callback_arg) {
    struct OctNode* node = getNode(tree, idx);

    if(node->contentType == CT_LEAF) {
        mult_scalar(&node->leaf.force, FORCE_MULT);
        add(&node->leaf.velocity, &node->leaf.force);
    }

    return true;
}

bool apply_velocity_on_node(struct OctTree* tree, node_idx_t idx, struct Extents* ext, void* callback_arg) {
    struct OctNode* node = getNode(tree, idx);

    if(node->contentType == CT_LEAF) {
        Velocity v = node->leaf.velocity;
        mult_scalar(&v, VELOCITY_MULT);
        add(&node->leaf.pos, &v);
        clamp_to_universe(&node->leaf.pos, &node->leaf.velocity);
    }

    return true;
}

bool rebalance_node(struct OctTree* tree, node_idx_t idx, struct Extents* ext, void* callback_arg) {
    struct OctNode* node = getNode(tree, idx);

    if(node->contentType == CT_LEAF) {
        if(!pos_inside(ext, &node->leaf.pos)) {
            struct Leaf leaf = node->leaf;
            setNodeEmpty(node);
            addLeaf(tree, &leaf);
        }
    }

    return true;
}

bool walk_leaves_proc(struct OctTree* tree, node_idx_t idx, struct Extents* ext, void* callback_arg) {
    struct OctNode* node = getNode(tree, idx);

    if(node->contentType == CT_LEAF) {
        void (*process_leaf)(struct OctNode* node) = callback_arg;
        process_leaf(node);
    }

    return true;
}

//// PUBLIC SECTION ////

struct OctTree* create_tree() {
    struct OctTree* tree = (struct OctTree*) malloc(sizeof(struct OctTree));
    tree->children = tree->children = malloc(sizeof(struct OctNode));
    tree->depth = 1;
    tree->elmsCount = 1;
    memset(tree->children, 0, sizeof(struct OctNode));
    return tree;
}

void destroy_tree(struct OctTree* tree) {
    free(tree->children);
    free(tree);
}

void addLeaf(struct OctTree* tree, struct Leaf* leaf) {
    node_idx_t idx = 0;
    depth_t depth = 0;

    struct Extents ext = get_max_extents();

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
                setNodeINode(tree, idx, &ext);
            }

            child_pos_idx_t pos_index = get_pos_index(&ext, &leaf->pos);
            idx = get_node_children(idx) + pos_index;
            update_extents(&ext, pos_index);
            depth++;
        }
    }
}

void createLeafWithPos(struct OctTree* tree, Pos* pos) {
    struct Leaf leaf;
    leaf.pos = *pos;
    addLeaf(tree, &leaf);
}

void calc_center_of_mass(struct OctTree* tree) {
    calc_node_center_of_mass(tree, 0);
}

void calc_force(struct OctTree* tree) {
    struct Extents ext = get_max_extents();
    walk_tree(tree, 0, &ext, calc_force_on_node, nullptr);
}

void apply_force(struct OctTree* tree) {
    struct Extents ext = get_max_extents();
    walk_tree(tree, 0, &ext, apply_force_on_node, nullptr);
}

void apply_velocity(struct OctTree* tree) {
    struct Extents ext = get_max_extents();
    walk_tree(tree, 0, &ext, apply_velocity_on_node, nullptr);
}

void rebalance(struct OctTree* tree) {
    struct Extents ext = get_max_extents();
    walk_tree(tree, 0, &ext, rebalance_node, nullptr);
}

void walk_leaves(struct OctTree* tree, void (*process_leaf)(struct OctNode* node)) {
    struct Extents ext = get_max_extents();
    walk_tree(tree, 0, &ext, walk_leaves_proc, process_leaf);
}