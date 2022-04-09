//
// Created by cyberpirate on 4/6/22.
//

#include "utils.h"
#include <math.h>
#include <stdio.h>

void set(struct Vec* vec, coord_t v) {
    vec->x = v;
    vec->y = v;
    vec->z = v;
}

void add(struct Vec* v1, struct Vec* v2) {
    v1->x += v2->x;
    v1->y += v2->y;
    v1->z += v2->z;
}

void sub(struct Vec* v1, struct Vec* v2) {
    v1->x -= v2->x;
    v1->y -= v2->y;
    v1->z -= v2->z;
}

void mult_scalar(struct Vec* vec, coord_t v) {
    vec->x *= v;
    vec->y *= v;
    vec->z *= v;
}

coord_t dist(struct Vec* p1, struct Vec* p2) {
    struct Vec t = *p1;
    sub(&t, p2);
    return vec_len(&t);
}

coord_t vec_len(struct Vec* vec) {
    return sqrt(
        vec->x*vec->x +
        vec->y*vec->y +
        vec->z*vec->z
    );
}

void norm(struct Vec* vec) {
    mult_scalar(vec, 1 / vec_len(vec));
}

struct Vec vec_dir(struct Vec* v1, struct Vec* v2) {
    struct Vec ret = *v2;
    sub(&ret, v1);
    norm(&ret);
    return ret;
}

void clamp_coord_to_universe(coord_t* v) {
    if(*v < -UNIVERSE_SIZE) {
        *v = -UNIVERSE_SIZE;
    }
    if(*v > UNIVERSE_SIZE) {
        *v = UNIVERSE_SIZE;
    }
}

void clamp_to_universe(Pos* pos) {
    clamp_coord_to_universe(&pos->x);
    clamp_coord_to_universe(&pos->y);
    clamp_coord_to_universe(&pos->z);
}

double interval(struct timespec start, struct timespec end)
{
    struct timespec temp;
    temp.tv_sec = end.tv_sec - start.tv_sec;
    temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    if (temp.tv_nsec < 0) {
        temp.tv_sec = temp.tv_sec - 1;
        temp.tv_nsec = temp.tv_nsec + 1000000000;
    }
    return (((double)temp.tv_sec) + ((double)temp.tv_nsec)*1.0e-9);
}