//
// Created by cyberpirate on 4/27/22.
//

#ifndef EC527_PROJECT_PROFILER_H
#define EC527_PROJECT_PROFILER_H

#ifdef PROFILE

#define init_profile(profile) double profile = 0

#define measure(profile, line, iters) { \
    struct timespec prof_start, prof_stop; \
    clock_gettime(CLOCK_REALTIME, &prof_start); \
    line;                                       \
    clock_gettime(CLOCK_REALTIME, &prof_stop);  \
    profile += interval(prof_start, prof_stop) / iters; \
}

#define print_profile(profile) printf(#profile ": %f\n", profile)

#else

#define init_profile(profile) {}

#define measure(profile, line, iters) line

#define print_profile(profile) {}

#endif

#endif //EC527_PROJECT_PROFILER_H
