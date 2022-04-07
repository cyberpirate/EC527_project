//
// Created by cyberpirate on 3/30/22.
//

#ifndef EC527_PROJECT_RAND_GEN_H
#define EC527_PROJECT_RAND_GEN_H

#include "params.h"
#include "utils.h"

/**
 * Reset the random number generator with the starting seed
 */
void reset_rand();

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

#endif //EC527_PROJECT_RAND_GEN_H
