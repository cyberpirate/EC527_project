//
// Created by cyberpirate on 3/30/22.
//

#include <malloc.h>
#include <memory.h>
#include <math.h>
#include "oct_tree.h"
#include "rand_gen.h"

uint8_t get_pos_index(Pos* minExt, Pos* maxExt, Pos* pos) {

    uint8_t ret = 0;
    Pos center;

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

    ret->maxExt.x = (UNIVERSE_SIZE + 0.1); // add some wiggle room for elements right on the edge
    ret->maxExt.y = (UNIVERSE_SIZE + 0.1);
    ret->maxExt.z = (UNIVERSE_SIZE + 0.1);

    ret->minExt.x = -(UNIVERSE_SIZE + 0.1);
    ret->minExt.y = -(UNIVERSE_SIZE + 0.1);
    ret->minExt.z = -(UNIVERSE_SIZE + 0.1);

    return ret;
}

void destroy_oct_node(struct OctNode* node) {
    dbgAssert(node->contentType == CT_EMPTY);

    free(node);
}

void explode_node(struct OctNode* node) {

    dbgAssert(node->contentType == CT_LEAVES);

    Pos center;

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
        node->nodes[i]->parent = node;
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

    coord_t d = 0;
    for(int i = 0; i < NODE_CHILDREN_NUM-1; i++) {
        d += dist(&leaves[i]->pos, &leaves[i+1]->pos);
    }
    if(d < SAME_POS_TOL) {
        for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
            Pos p = rand_pos();
            norm(&p);
            mult_scalar(&p, (node->maxExt.x - node->minExt.x)*SAME_POS_MOV);
            add(&leaves[i]->pos, &p);

            clamp(node->minExt.x, &leaves[i]->pos.x, node->maxExt.x);
            clamp(node->minExt.y, &leaves[i]->pos.y, node->maxExt.y);
            clamp(node->minExt.z, &leaves[i]->pos.z, node->maxExt.z);
        }
    }

    for(int i = 0; i < NODE_CHILDREN_NUM; i++)
        add_leaf(node, leaves[i]);

    node->size = NODE_CHILDREN_NUM;
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

    node->size++;

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
            node->size++;
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

    node->size--;

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

uint8_t leaf_inside(struct OctNode* node, struct Leaf* leaf) {
    return
        node->minExt.x <= leaf->pos.x && leaf->pos.x < node->maxExt.x &&
        node->minExt.y <= leaf->pos.y && leaf->pos.y < node->maxExt.y &&
        node->minExt.z <= leaf->pos.z && leaf->pos.z < node->maxExt.z;
}

void calc_center_of_mass(struct OctNode* node) {
    if(node->contentType == CT_EMPTY) {
        node->centerOfMass.x = (node->maxExt.x + node->minExt.x) / 2;
        node->centerOfMass.y = (node->maxExt.y + node->minExt.y) / 2;
        node->centerOfMass.z = (node->maxExt.z + node->minExt.z) / 2;
    }

    if(node->contentType == CT_LEAVES) {
        node->centerOfMass.x = 0;
        node->centerOfMass.y = 0;
        node->centerOfMass.z = 0;

        for(int i = 0; i < LEAF_CHILDREN_NUM; i++) {
            if(node->leaves[i] == nullptr)
                break;

            node->centerOfMass.x += node->leaves[i]->pos.x / (float) node->size;
            node->centerOfMass.y += node->leaves[i]->pos.y / (float) node->size;
            node->centerOfMass.z += node->leaves[i]->pos.z / (float) node->size;
        }
    }

    if(node->contentType == CT_NODES) {
        node->centerOfMass.x = 0;
        node->centerOfMass.y = 0;
        node->centerOfMass.z = 0;

        for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
            node->centerOfMass.x += node->nodes[i]->centerOfMass.x * (float) node->nodes[i]->size / (float) node->size;
            node->centerOfMass.y += node->nodes[i]->centerOfMass.y * (float) node->nodes[i]->size / (float) node->size;
            node->centerOfMass.z += node->nodes[i]->centerOfMass.z * (float) node->nodes[i]->size / (float) node->size;
        }
    }
}

void calc_force_leaf(struct OctNode* node, struct Leaf* leaf) {
    if(node->contentType == CT_EMPTY) return;

    if(node->contentType == CT_LEAVES) {
        for(int i = 0; i < LEAF_CHILDREN_NUM; i++) {
            if(node->leaves[i] == nullptr) break;
            if(node->leaves[i] == leaf) continue;

            coord_t d = dist(&leaf->pos, &node->leaves[i]->pos);
            if(d == 0) continue;
            coord_t f = (G*2)/(d*d);
            Force fVec = vec_dir(&leaf->pos, &node->leaves[i]->pos);
            mult_scalar(&fVec, f);

            add(&leaf->force, &fVec);
        }
    }

    if(node->contentType == CT_NODES) {
        coord_t s = node->maxExt.x - node->minExt.x;
        s = s > 0 ? s : -s;

        coord_t d = dist(&node->centerOfMass, &leaf->pos);

        if(s/d < SD_THRESHOLD) { // node is far enough away

            coord_t f = (G*(node->size + 1))/(d*d);
            Force fVec = vec_dir(&leaf->pos, &node->centerOfMass);
            mult_scalar(&fVec, f);

            add(&leaf->force, &fVec);

        } else { // node is too close
            for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
                calc_force_leaf(node->nodes[i], leaf);
            }
        }
    }
}

void calc_force_internal(struct OctNode* root, struct OctNode* node) {
    if(node->contentType == CT_EMPTY) return;

    if(node->contentType == CT_LEAVES) {
        for(int i = 0; i < LEAF_CHILDREN_NUM; i++) {
            if(node->leaves[i] == nullptr) break;

            set(&node->leaves[i]->force, 0);
            calc_force_leaf(root, node->leaves[i]);
        }
    }

    if(node->contentType == CT_NODES) {
        for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
            calc_force_internal(root, node->nodes[i]);
        }
    }
}

void calc_force(struct OctNode* node) {
    calc_force_internal(node, node);
}

void apply_force(struct OctNode* node) {
    if(node->contentType == CT_EMPTY) return;

    if(node->contentType == CT_LEAVES) {
        for(int i = 0; i < LEAF_CHILDREN_NUM; i++) {
            if(node->leaves[i] == nullptr) break;

            mult_scalar(&node->leaves[i]->force, FORCE_MULT);
            add(&node->leaves[i]->velocity, &node->leaves[i]->force);
        }
    }

    if(node->contentType == CT_NODES) {
        for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
            apply_force(node->nodes[i]);
        }
    }
}

void apply_velocity(struct OctNode* node) {
    if(node->contentType == CT_EMPTY) return;

    if(node->contentType == CT_LEAVES) {
        for(int i = 0; i < LEAF_CHILDREN_NUM; i++) {
            if(node->leaves[i] == nullptr) break;

            Velocity v = node->leaves[i]->velocity;
            mult_scalar(&v, VELOCITY_MULT);
            add(&node->leaves[i]->pos, &v);
            clamp_to_universe(&node->leaves[i]->pos, &node->leaves[i]->velocity);
        }
    }

    if(node->contentType == CT_NODES) {
        for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
            apply_velocity(node->nodes[i]);
        }
    }
}

void rebalance_internal(struct OctNode* target, struct Leaf* leaf) {
    if(leaf_inside(target, leaf)) return;

    remove_leaf(target, leaf);

    struct OctNode* parent = target->parent;
    while(parent != nullptr) {
        parent->size--;
        parent = parent->parent;
    }

    struct OctNode* addition_target = target;
    while(!leaf_inside(addition_target, leaf)) {
        addition_target = addition_target->parent;
        dbgAssert(addition_target != nullptr);
    }

    add_leaf(addition_target, leaf);

    parent = addition_target->parent;
    while(parent != nullptr) {
        parent->size++;
        parent = parent->parent;
    }

    target = target->parent;
    while(target != addition_target && target != nullptr) {

        if(target->contentType == CT_NODES && target->size == 0) {
            for(int i = 0; i < NODE_CHILDREN_NUM; i++) {
                destroy_oct_node(target->nodes[i]);
                target->nodes[i] = nullptr;
            }
            target->contentType = CT_EMPTY;
        } else {
            break;
        }

        target = target->parent;
    }
}

void rebalance(struct OctNode* node) {
    if(node->contentType == CT_EMPTY) return;

    if(node->contentType == CT_LEAVES) {
        for(int i = LEAF_CHILDREN_NUM-1; i >= 0; i--) { // iterate in reverse so no leaves will be skipped when removing the current node
            if(node->leaves[i] == nullptr) continue;
            rebalance_internal(node, node->leaves[i]);
        }
    }

    if(node->contentType == CT_NODES) {
        for(int i = 0; i < NODE_CHILDREN_NUM && node->contentType == CT_NODES; i++) {
            rebalance(node->nodes[i]);
        }
    }
}