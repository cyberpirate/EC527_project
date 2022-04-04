
#include "params.h"
#include "oct_tree.h"
#include "linked_stack.h"
#include "rand_gen.h"

int main(int argc, char *argv[])
{
    reset_rand();

    struct Leaf* leaves[LEAF_COUNT];
    for(int i = 0; i < LEAF_COUNT; i++) {
        leaves[i] = create_leaf();
        leaves[i]->pos = rand_pos();
    }

    return 0;
}
