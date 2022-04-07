//
// Created by cyberpirate on 3/30/22.
//

#include "rand_gen.h"
#include "oct_tree.h"
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

Pos rand_pos() {
    Pos ret;
    ret.x = rand_coord();
    ret.y = rand_coord();
    ret.z = rand_coord();
    return ret;
}
