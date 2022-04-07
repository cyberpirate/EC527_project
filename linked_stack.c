//
// Created by cyberpirate on 4/2/22.
//

#include <malloc.h>
#include "linked_stack.h"
#include "oct_tree.h"

struct LinkedStackRoot* create_linked_stack_root() {
    struct LinkedStackRoot* root = malloc(sizeof(struct LinkedStackRoot));
    root->root = nullptr;
    root->count = 0;
    return root;
}

void add_node(struct LinkedStackRoot* root, struct Leaf* value) {
    struct LinkedStackNode* newNode = malloc(sizeof(struct LinkedStackNode));
    newNode->next = root->root;
    newNode->value = value;
    root->root = newNode;
    root->count++;
}

struct Leaf* get_and_pop_value(struct LinkedStackRoot* root) {
    dbgAssert(root->count > 0);
    struct LinkedStackNode* node = root->root;
    struct Leaf* value = node->value;
    root->root = node->next;
    root->count--;
    free(node);
    return value;
}

void destroy_linked_stack_root(struct LinkedStackRoot* root) {
    dbgAssert(root->count == 0);

    free(root);
}

void calculate_average_pos(struct LinkedStackRoot* root, Pos* pos) {
    pos->x = 0;
    pos->y = 0;
    pos->z = 0;

    struct LinkedStackNode* next = root->root;

    while(next != nullptr) {
        pos->x += next->value->pos.x;
        pos->y += next->value->pos.y;
        pos->z += next->value->pos.z;
        next = next->next;
    }

    pos->x /= (coord_t) root->count;
    pos->y /= (coord_t) root->count;
    pos->z /= (coord_t) root->count;
}