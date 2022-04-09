//
// Created by cyberpirate on 4/6/22.
//

#ifndef EC527_PROJECT_UTILS_H
#define EC527_PROJECT_UTILS_H

#include "params.h"
#include <time.h>


struct Vec {
    coord_t x;
    coord_t y;
    coord_t z;
};

typedef struct Vec Pos;
typedef struct Vec Force;

/**
 * Set vector components to value
 * @param vec
 * @param v
 */
void set(struct Vec* vec, coord_t v);

/**
 * Add v2 to v1
 * @param v1
 * @param v2
 */
void add(struct Vec* v1, struct Vec* v2);

/**
 * Subtract v2 from v1
 * @param v1
 * @param v2
 */
void sub(struct Vec* v1, struct Vec* v2);

/**
 * Multiply vector by scalar
 * @param vec
 * @param v
 */
void mult_scalar(struct Vec* vec, coord_t v);

/**
 * Calcuate dist between two points
 * @param p1
 * @param p2
 * @return
 */
coord_t dist(struct Vec* p1, struct Vec* p2);

/**
 * Get the length of a vector
 * @param pos
 * @return
 */
coord_t vec_len(struct Vec* vec);

/**
 * Normalize a vector
 * @param pos
 */
void norm(struct Vec* vec);

/**
 * Return new normalized vector pointing from v1 to v2
 * @param v1
 * @param v2
 * @return
 */
struct Vec vec_dir(struct Vec* v1, struct Vec* v2);

/**
 * Clamp coord to world size
 * @param v
 */
void clamp_coord_to_universe(coord_t* v);

/**
 * Clamp position to ensure it does not leave the world bounds
 * @param pos
 */
void clamp_to_universe(Pos* pos);

double interval(struct timespec start, struct timespec end);

#endif //EC527_PROJECT_UTILS_H
