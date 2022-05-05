//
// Created by cyberpirate on 3/30/22.
//

#ifndef EC527_PROJECT_RAND_GEN_H
#define EC527_PROJECT_RAND_GEN_H

#include "params.cuh"
#include "utils.cuh"

/**
 * Reset the random number generator with the starting seed
 */
void reset_rand();

/**
 * generate a +- limit
 * @param limit
 * @return
 */
coord_t rand_value(coord_t limit);

/**
 * Get a random coordinate within bounds
 * @return
 */
coord_t rand_coord();

/**
 * Get a random position
 * @return
 */
Pos rand_pos();

/**
 * Get random position inside a torus
 * @return
 */
Pos rand_torus(coord_t outer_radius, coord_t inner_radius);

/**
 * Create a velocity from a position for a orbit
 * @param pos
 * @return
 */
Velocity vel_from_pos(Pos* pos);

#endif //EC527_PROJECT_RAND_GEN_H
