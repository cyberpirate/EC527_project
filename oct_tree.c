//
// Created by cyberpirate on 3/30/22.
//

#include "oct_tree.h"
#include "rand_gen.h"
#include <malloc.h>
#include <math.h>
#include <string.h>

//region idx tree traversal

struct Leaf* getLeaf(struct OctTree* tree, node_idx_t node_idx, child_pos_idx_t leaf_idx) {
    dbgAssert(getNode(tree, node_idx)->contentType == CT_LEAVES);
    dbgAssert(getNode(tree, node_idx)->leaves[leaf_idx] < tree->leaf_count);
    return &tree->leaves[getNode(tree, node_idx)->leaves[leaf_idx]];
}

struct OctNode* getNode(struct OctTree* tree, node_idx_t node_idx) {
    depth_t depth_idx = get_depth_for_idx(node_idx);
    dbgAssert(depth_idx < tree->depth_count);
    node_idx_t node_idx_in_depth = node_idx - idx_start_for_depth(depth_idx);
    dbgAssert(((node_idx_in_depth+1) * sizeof(struct OctNode))  <= malloc_usable_size(tree->depth[depth_idx]));
    return &tree->depth[depth_idx][node_idx_in_depth];
}

node_idx_t depth_size(depth_t depth) {
    node_idx_t idx = 1;
    for(depth_t i = 0; i < depth; i++) {
        idx *= NODE_CHILD_COUNT;
    }
    return idx;
}

node_idx_t array_size_for_depth(depth_t depth) {
    if(depth == 0) return depth_size(depth);
    return depth_size(depth) + array_size_for_depth(depth-1);
}

node_idx_t idx_start_for_depth(depth_t depth) {
    if(depth == 0) return 0;
    return array_size_for_depth(depth-1);
}

depth_t get_depth_for_idx(node_idx_t idx) {
    depth_t depth = 1;
    node_idx_t nextIdx = idx_start_for_depth(depth);
    while(idx >= nextIdx) {
        depth++;
        nextIdx = idx_start_for_depth(depth);
    }
    return depth-1;
}

node_idx_t get_node_children(node_idx_t idx) {
    depth_t depth = get_depth_for_idx(idx);
    node_idx_t depth_start = idx_start_for_depth(depth);
    node_idx_t next_depth_start = idx_start_for_depth(depth+1);
    return next_depth_start + (idx-depth_start)*NODE_CHILD_COUNT;
}

node_idx_t get_node_parent(node_idx_t idx) {
    depth_t depth = get_depth_for_idx(idx);
    node_idx_t pos = (idx - idx_start_for_depth(depth)) / NODE_CHILD_COUNT;
    return idx_start_for_depth(depth-1) + pos;
}

void set_tree_depth(struct OctTree* tree, depth_t depth_count) {
    dbgAssert(tree->depth_count < depth_count);
    dbgAssert(depth_count <= DEPTH_LIMIT);

    depth_t old_depth = tree->depth_count;
    struct OctNode** old_depth_ptr = tree->depth;

    tree->depth = calloc(depth_count, sizeof(struct OctNode*));
    tree->depth_count = depth_count;
    memset(tree->depth, 0, depth_count*sizeof(struct OctNode*));

    if(old_depth_ptr != nullptr) {
        memcpy(tree->depth, old_depth_ptr, old_depth * sizeof(struct OctNode*));
        free(old_depth_ptr);
    }

    for(depth_t i = old_depth; i < tree->depth_count; i++) {
        tree->depth[i] = calloc(depth_size(i), sizeof(struct OctNode));
        memset(tree->depth[i], 0, depth_size(i) * sizeof(struct OctNode));
        dbgAssert(depth_size(i) * sizeof(struct OctNode*) <= malloc_usable_size(tree->depth[i]));
    }
}

void walk_tree(struct OctTree* tree, node_idx_t idx, struct Extents* ext, bool (*process_callback)(struct OctTree* tree, node_idx_t idx, struct Extents* ext, void* callbackArg), void* callback_arg) {
    if(getNode(tree, idx)->contentType == CT_EMPTY) return;

    if(!process_callback(tree, idx, ext, callback_arg)) return;

    if(getNode(tree, idx)->contentType == CT_NODES) {
        node_idx_t children_idx = get_node_children(idx);

        for(int i = 0; i < NODE_CHILD_COUNT; i++) {
            node_idx_t child_idx = children_idx + i;
            struct Extents childExt = *ext;
            update_extents(&childExt, child_idx);
            walk_tree(tree, child_idx, &childExt, process_callback, callback_arg);
        }
    }
}

//endregion idx tree traversal

//region oct node operations

void setNodeEmpty(struct OctNode* node) {
    memset(node, 0, sizeof(struct OctNode));
}

void setNodeToLeafNode(struct OctNode* node) {
    node->contentType = CT_LEAVES;
    node->size = 0;
}

void addLeafToLeafNode(struct OctNode* node, leaf_idx_t idx) {
    dbgAssert(node->size < LEAF_CHILD_COUNT);
    node->leaves[node->size] = idx;
    node->size++;
}

void removeLeafFromNode(struct OctNode* node, child_pos_idx_t idx) {
    dbgAssert(node->size > 0);

    node->size--;
    if(node->size == 0) {
        setNodeEmpty(node);
        return;
    }

    for(child_pos_idx_t i = idx; i < node->size-1; i++) {
        node->leaves[i] = node->leaves[i+1];
    }
}

void scatterLeavesInNode(struct OctTree* tree, node_idx_t idx, struct Extents* ext) {
    dbgAssert(getNode(tree, idx)->contentType == CT_LEAVES);

    child_pos_idx_t sizeTarget = LEAF_CHILD_COUNT - MAX_SCATTER;

    while(true) {
        rebalance_node(tree, idx, ext, nullptr);

        if(getNode(tree, idx)->size <= sizeTarget) break;

        getLeaf(tree, idx, 0)->pos = rand_pos();
    }
}

bool setNodeToInternalNode(struct OctTree* tree, node_idx_t idx, struct Extents* ext) {

    if(tree->depth_count == DEPTH_LIMIT) {
//        scatterLeavesInNode(tree, idx, ext);
        return false;
    }

    node_idx_t size = getNode(tree, idx)->size;
    leaf_idx_t leaves[LEAF_CHILD_COUNT];
    for(leaf_idx_t i = 0; i < LEAF_CHILD_COUNT; i++)
        leaves[i] = getNode(tree, idx)->leaves[i];

    getNode(tree, idx)->contentType = CT_NODES;
    getNode(tree, idx)->size = 0;

    for(node_idx_t i = 0; i < size; i++) {
        struct Extents childExt = *ext;
        addLeafToNode(tree, leaves[i], idx, &childExt);
    }

    return true;
}

//endregion oct node operations

//region add leaf

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

bool addLeafToNode(struct OctTree* tree, leaf_idx_t leaf_idx, node_idx_t idx, struct Extents* ext) {

    depth_t idx_depth = get_depth_for_idx(idx);
    if(idx_depth >= tree->depth_count) {
        set_tree_depth(tree, idx_depth + 1);
    }

    Pos* leafPos = &tree->leaves[leaf_idx].pos;
//    dbgAssert(pos_inside(ext, leafPos));

    if(getNode(tree, idx)->contentType == CT_EMPTY) {
        setNodeToLeafNode(getNode(tree, idx));
    }

    if(getNode(tree, idx)->contentType == CT_LEAVES) {
        if(getNode(tree, idx)->size < LEAF_CHILD_COUNT) {
            addLeafToLeafNode(getNode(tree, idx), leaf_idx);
            return true;
        } else {
            if(!setNodeToInternalNode(tree, idx, ext)) {
                return false;
            }
//            if(getNode(tree, idx)->contentType != CT_NODES) {
//                // hit depth limit, node has scattered so add
//                if(getNode(tree, idx)->contentType == CT_EMPTY)
//                    setNodeToLeafNode(getNode(tree, idx));
//                addLeafToLeafNode(getNode(tree, idx), leaf_idx);
//                return;
//            }
        }
    }

    if(getNode(tree, idx)->contentType == CT_NODES) {
        child_pos_idx_t posIdx = get_pos_index(ext, leafPos);
        update_extents(ext, posIdx);
        if(addLeafToNode(tree, leaf_idx, get_node_children(idx) + posIdx, ext)) {
            getNode(tree, idx)->size++;
            return true;
        }

        return false;
    }

    dbgAssert(false);
}
//endregion add leaf

//region center of mass

void calc_node_center_of_mass(struct OctTree* tree, node_idx_t idx) {
    if(getNode(tree, idx)->contentType == CT_EMPTY) return;

    if(getNode(tree, idx)->contentType == CT_LEAVES) {
        getNode(tree, idx)->centerOfMass.x = 0;
        getNode(tree, idx)->centerOfMass.y = 0;
        getNode(tree, idx)->centerOfMass.z = 0;

        for(int i = 0; i < getNode(tree, idx)->size; i++) {
            struct Leaf* leaf = getLeaf(tree, idx, i);
            getNode(tree, idx)->centerOfMass.x += leaf->pos.x / (float) getNode(tree, idx)->size;
            getNode(tree, idx)->centerOfMass.y += leaf->pos.y / (float) getNode(tree, idx)->size;
            getNode(tree, idx)->centerOfMass.z += leaf->pos.z / (float) getNode(tree, idx)->size;
        }

        return;
    }

    if(getNode(tree, idx)->contentType == CT_NODES) {
        getNode(tree, idx)->centerOfMass.x = 0;
        getNode(tree, idx)->centerOfMass.y = 0;
        getNode(tree, idx)->centerOfMass.z = 0;

        node_idx_t children_start = get_node_children(idx);
        for(int i = 0; i < NODE_CHILD_COUNT; i++) {
            calc_node_center_of_mass(tree, children_start+i);

            getNode(tree, idx)->centerOfMass.x += (getNode(tree, children_start+i)->centerOfMass.x * (float) getNode(tree, children_start+i)->size) / (float) getNode(tree, idx)->size;
            getNode(tree, idx)->centerOfMass.y += (getNode(tree, children_start+i)->centerOfMass.y * (float) getNode(tree, children_start+i)->size) / (float) getNode(tree, idx)->size;
            getNode(tree, idx)->centerOfMass.z += (getNode(tree, children_start+i)->centerOfMass.z * (float) getNode(tree, children_start+i)->size) / (float) getNode(tree, idx)->size;
        }
        return;
    }

    dbgAssert(false);
}

//endregion center of mass

//region calc force on node
bool calc_force_for_node(struct OctTree* tree, node_idx_t idx, struct Extents* ext, void* callback_arg) {
    struct Leaf* leaf = (struct Leaf*) callback_arg;

    coord_t s = ext->maxExt.x - ext->minExt.x;
    s = s > 0 ? s : -s;

    coord_t d = dist(&getNode(tree, idx)->centerOfMass, &leaf->pos);

    if(s/d < SD_THRESHOLD) { // node is far enough away

        coord_t f = (G*(getNode(tree, idx)->size + 1))/(d*d);
        Force fVec = vec_dir(&leaf->pos, &getNode(tree, idx)->centerOfMass);
        mult_scalar(&fVec, f);

        add(&leaf->force, &fVec);

        return false;
    } else { // node is too close
        if(getNode(tree, idx)->contentType == CT_LEAVES) {
            for(leaf_idx_t i = 0; i < getNode(tree, idx)->size; i++) {
                struct Leaf* otherLeaf = getLeaf(tree, idx, i);
                coord_t d = dist(&leaf->pos, &otherLeaf->pos);
                if(d == 0) continue;
                coord_t f = (G*2)/(d*d);
                Force fVec = vec_dir(&leaf->pos, &otherLeaf->pos);
                mult_scalar(&fVec, f);

                add(&leaf->force, &fVec);
            }
        } else {
            return true;
        }
    }

    return true;
}

void calc_force_on_leaf(struct OctTree* tree, struct Leaf* leaf) {
    struct Extents rootExt = get_max_extents();
    walk_tree(tree, 0, &rootExt, calc_force_for_node, leaf);
}
//endregion calc force on node

//region apply force
void apply_force_on_leaf(struct OctTree* tree, struct Leaf* leaf) {
    mult_scalar(&leaf->force, FORCE_MULT);
    add(&leaf->velocity, &leaf->force);
}
//endregion apply force

//region apply velocity
void apply_velocity_on_leaf(struct OctTree* tree, struct Leaf* leaf) {
    Velocity v = leaf->velocity;
    mult_scalar(&v, VELOCITY_MULT);
    add(&leaf->pos, &v);
    clamp_to_universe(&leaf->pos, &leaf->velocity);
}
//endregion apply velocity

//region rebalance
bool rebalance_node(struct OctTree* tree, node_idx_t idx, struct Extents* ext, void* callback_arg) {
    for(node_idx_t i = getNode(tree, idx)->size-1; getNode(tree, idx)->contentType == CT_LEAVES; i--) {
        struct Leaf* leaf = getLeaf(tree, idx, i);
        if(!pos_inside(ext, &leaf->pos)) {
            leaf_idx_t leafIdx = getNode(tree, idx)->leaves[i];

            removeLeafFromNode(getNode(tree, idx), i);

            node_idx_t parentIdx = get_node_parent(idx);
            while(true) {
                getNode(tree, parentIdx)->size--;
                if(getNode(tree, parentIdx)->size == 0) setNodeEmpty(getNode(tree, parentIdx));
                if(parentIdx == 0) break;
                parentIdx = get_node_parent(parentIdx);
            }

            addLeaf(tree, leafIdx);
        }
        if(i == 0) break;
    }

    return true;
}
//endregion rebalance

////// PUBLIC SECTION ////

struct OctTree* create_tree(leaf_idx_t leaf_count) {
    struct OctTree* tree = (struct OctTree*) malloc(sizeof(struct OctTree));

    tree->leaves = calloc(leaf_count, sizeof(struct Leaf));
    memset(tree->leaves, 0, leaf_count*sizeof(struct Leaf));
    tree->leaf_count = leaf_count;

    tree->depth = nullptr;
    tree->depth_count = 0;
    set_tree_depth(tree, 1);

    return tree;
}

void destroy_tree(struct OctTree* tree) {
    free(tree->leaves);
    for(depth_t i = 0; i < tree->depth_count; i++) {
        free(tree->depth[i]);
    }
    free(tree->depth);
    free(tree);
}

void add_leaves_to_tree(struct OctTree* tree) {
    for(leaf_idx_t i = 0; i < tree->leaf_count; i++) {
        addLeaf(tree, i);
    }
}

void addLeaf(struct OctTree* tree, leaf_idx_t leaf_idx) {
    while(true) {
        struct Extents ext = get_max_extents();
        if(addLeafToNode(tree, leaf_idx, 0, &ext)) {
            break;
        }
        tree->leaves[leaf_idx].pos = rand_pos();
    }
}

void calc_center_of_mass(struct OctTree* tree) {
    calc_node_center_of_mass(tree, 0);
}

void calc_force(struct OctTree* tree) {
    walk_leaves(tree, calc_force_on_leaf);
}

void apply_force(struct OctTree* tree) {
    walk_leaves(tree, apply_force_on_leaf);
}

void apply_velocity(struct OctTree* tree) {
    walk_leaves(tree, apply_velocity_on_leaf);
}

void rebalance(struct OctTree* tree) {
    struct Extents ext = get_max_extents();
    walk_tree(tree, 0, &ext, rebalance_node, nullptr);
}

void walk_leaves(struct OctTree* tree, void (*process_leaf)(struct OctTree* tree, struct Leaf* leaf)) {
    for(leaf_idx_t i = 0; i < tree->leaf_count; i++) {
        process_leaf(tree, &tree->leaves[i]);
    }
}