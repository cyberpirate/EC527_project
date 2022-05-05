//
// Created by cyberpirate on 4/6/22.
//

#ifndef EC527_PROJECT_UTILS_H
#define EC527_PROJECT_UTILS_H

#include "params.cuh"
#include <time.h>
#include <stdint-gcc.h>


struct Vec {
    coord_t x;
    coord_t y;
    coord_t z;
};

typedef struct Vec Pos;
typedef struct Vec Force;
typedef struct Vec Velocity;

/**
 * Set vector components to value
 * @param vec
 * @param v
 */
__host__ __device__ void set(struct Vec* vec, coord_t v);

/**
 * Add v2 to v1
 * @param v1
 * @param v2
 */
__host__ __device__ void add(struct Vec* v1, struct Vec* v2);

/**
 * Subtract v2 from v1
 * @param v1
 * @param v2
 */
__host__ __device__ void sub(struct Vec* v1, struct Vec* v2);

/**
 * Multiply vector by scalar
 * @param vec
 * @param v
 */
__host__ __device__ void mult_scalar(struct Vec* vec, coord_t v);

/**
 * Calcuate dist between two points
 * @param p1
 * @param p2
 * @return
 */
__host__ __device__ coord_t dist(struct Vec* p1, struct Vec* p2);

/**
 * Get the length of a vector
 * @param pos
 * @return
 */
__host__ __device__ coord_t vec_len(struct Vec* vec);

/**
 * Normalize a vector
 * @param pos
 */
__host__ __device__ void norm(struct Vec* vec);

/**
 * Return new normalized vector pointing from v1 to v2
 * @param v1
 * @param v2
 * @return
 */
__host__ __device__ struct Vec vec_dir(struct Vec* v1, struct Vec* v2);

/**
 * Clamp coord to specified range
 * @param min
 * @param v
 * @param max
 * @return
 */
__host__ __device__ uint8_t clamp(coord_t min, coord_t* v, coord_t max);

/**
 * Clamp coord to world size
 * @param v
 * @return true if clamped
 */
__host__ __device__ uint8_t clamp_coord_to_universe(coord_t* v);

/**
 * Clamp position to ensure it does not leave the world bounds
 * @param pos
 */
__host__ __device__ void clamp_to_universe(Pos* pos, Velocity* vel);

double interval(struct timespec start, struct timespec end);

void printTimeLeft(double timePast, int i);

#endif //EC527_PROJECT_UTILS_H
