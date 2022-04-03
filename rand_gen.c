//
// Created by cyberpirate on 3/30/22.
//

#include "rand_gen.h"
#include "quad_tree.h"
#include <stdlib.h>

void reset_rand() {
    srand(RAND_SEED);
}

coord_t rand_coord() {
    coord_t ret = (coord_t) rand();
    ret /= (float) (RAND_MAX/2);
    ret -= 1;
    ret *= UNIVERSE_SIZE;
    return ret;
}

struct Pos rand_pos() {
    struct Pos ret;
    ret.x = rand_coord();
    ret.y = rand_coord();
    ret.z = rand_coord();
    return ret;
}
