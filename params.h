//
// Created by cyberpirate on 3/30/22.
//

#ifndef EC527_PROJECT_PARAMS_H
#define EC527_PROJECT_PARAMS_H

typedef float coord_t;

#define UNIVERSE_SIZE 100
#define RAND_SEED 506


#define nullptr 0

#ifdef DEBUG

#include <assert.h>
#define dbgAssert(...) assert(__VA_ARGS__)

#else

#define dbgAssert(...) {}

#endif

#endif //EC527_PROJECT_PARAMS_H
