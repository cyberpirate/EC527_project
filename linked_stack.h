//
// Created by cyberpirate on 4/2/22.
//

#ifndef EC527_PROJECT_LINKED_STACK_H
#define EC527_PROJECT_LINKED_STACK_H

#include <stdint-gcc.h>
#include "params.h"
#include "utils.h"

struct Leaf;

struct LinkedStackNode {
    struct LinkedStackNode* next;
    struct Leaf* value;
};

struct LinkedStackRoot {
    struct LinkedStackNode* root;
    uint32_t count;
};

/**
 * Create a new linked list
 * @return
 */
struct LinkedStackRoot* create_linked_stack_root();

/**
 * Add to a linked list
 * @param value
 */
void add_node(struct LinkedStackRoot* root, struct Leaf* value);

/**
 * Get and pop the leaf off the linked stack
 * @param root
 * @return
 */
struct Leaf* get_and_pop_value(struct LinkedStackRoot* root);

/**
 * Destroy the root
 * @param node
 */
void destroy_linked_stack_root(struct LinkedStackRoot* root);

/**
 * Calculate the average position of the linked stack
 * @param root
 * @param pos
 * @return
 */
void calculate_average_pos(struct LinkedStackRoot* root, Pos* pos);

#endif //EC527_PROJECT_LINKED_STACK_H
