//
// Created by cyberpirate on 3/30/22.
//

#ifndef EC527_PROJECT_PARAMS_H
#define EC527_PROJECT_PARAMS_H

typedef double coord_t;

#define UNIVERSE_SIZE 100
#define SD_THRESHOLD 0.5
#define RAND_SEED 506
#define LEAF_COUNT 1000
#define ITERS 1000
#define FRAME_STEP 0.5

#define G (6.67E-11)
#define nullptr 0

#ifdef DEBUG

#include <assert.h>
#define dbgAssert(...) assert(__VA_ARGS__)

#else

#define dbgAssert(...) {}

#endif

#endif //EC527_PROJECT_PARAMS_H
