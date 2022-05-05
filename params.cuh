//
// Created by cyberpirate on 3/30/22.
//

#ifndef EC527_PROJECT_PARAMS_H
#define EC527_PROJECT_PARAMS_H

#include <cstdio>
#include <cstdlib>

typedef double coord_t;

#define UNIVERSE_SIZE 100
#define UNIVERSE_CLAMP_MOD 0.001
#define SD_THRESHOLD 0.5
#define RAND_SEED 506
#define POINT_COUNT 100000
#define ITERS (30*60)
#define FORCE_MULT 1
#define VELOCITY_MULT 1
#define SAME_POS_TOL 0.1
#define SAME_POS_MOV 1
#define MIN_NODE_SIZE 0.5
#define DEPTH_LIMIT 9
#define MAX_SCATTER 1
#define TORUS_OUTER 75
#define TORUS_INNER 25
#define TORUS_VEL 1
#define THREAD_COUNT 16
#define DEPTH_TO_PROC_SIZE 5000

#define G (6.67E-11*1E8)
#define nullptr 0

#ifdef DEBUG

#include <assert.h>
#define dbgAssert(...) assert(__VA_ARGS__)

#else

#define dbgAssert(...) {}

#endif

// Assertion to check for errors
#define CUDA_SAFE_CALL(ans) { gpuAssert((ans), (char *)__FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, char *file, int line, bool abort=true)
{
  if (code != cudaSuccess)
  {
    fprintf(stderr, "CUDA_SAFE_CALL: %s %s %d\n",
                                       cudaGetErrorString(code), file, line);
    if (abort) exit(code);
  }
}

#endif //EC527_PROJECT_PARAMS_H
