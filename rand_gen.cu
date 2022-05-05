//
// Created by cyberpirate on 3/30/22.
//

#include "rand_gen.cuh"
#include "oct_tree.cuh"
#include <stdlib.h>
#include <math.h>

#define PI 3.14

void reset_rand() {
    srand(RAND_SEED);
}

coord_t rand_value(coord_t limit) {
    coord_t ret = (coord_t) rand();
    ret /= (float) (RAND_MAX/2);
    ret -= 1;
    ret *= limit;
    return ret;
}

coord_t rand_coord() {
    return rand_value(UNIVERSE_SIZE);
}

Pos rand_pos() {
    Pos ret;
    ret.x = rand_coord();
    ret.y = rand_coord();
    ret.z = rand_coord();
    return ret;
}

Pos rand_torus(coord_t outer_radius, coord_t inner_radius) {
    Pos ret;

    coord_t diff = rand_value(inner_radius);
    coord_t r = outer_radius + diff;
    coord_t theta = rand_value(PI) + PI;

    ret.x = r*cos(theta);
    ret.y = r*sin(theta);
    ret.z = rand_value(inner_radius*cos((diff/inner_radius)*(PI/2)));

    return ret;
}

Velocity vel_from_pos(Pos* pos) {
    Velocity ret;
    ret.x = -pos->y;
    ret.y = pos->x;
    ret.z = 0;
    return ret;
}
