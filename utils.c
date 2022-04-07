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