
#include <stdio.h>
#include <bits/time.h>
#include <time.h>
#include "params.h"
#include "oct_tree.h"
#include "rand_gen.h"
#include "utils.h"

#define PRINT_CALCULATING

FILE* outputFile;

void print_leaf_pos(struct OctTree* tree, struct Leaf* leaf) {
    fprintf(outputFile, ",%f,%f,%f", leaf->pos.x, leaf->pos.y, leaf->pos.z);
}

void set_leaf_pos(struct OctTree* tree, struct Leaf* leaf) {
    leaf->pos = rand_torus(TORUS_OUTER, TORUS_INNER);
    leaf->velocity = vel_from_pos(&leaf->pos);
    mult_scalar(&leaf->velocity, TORUS_VEL/UNIVERSE_SIZE);
}


int main(int argc, char *argv[])
{
    reset_rand();

    struct OctTree* tree = create_tree(POINT_COUNT);
    double times[ITERS + 1];
    struct timespec time_start, time_stop;
    struct timespec tot_time_start, tot_time_stop;
    clock_gettime(CLOCK_REALTIME, &tot_time_start);

    outputFile = fopen("output.csv", "w");

    if(outputFile == nullptr) {
        printf("Error, can't open output.csv\n");
        return 1;
    }

    fprintf(outputFile, "frame");
    for(int i = 0; i < POINT_COUNT; i++)
        fprintf(outputFile, ",%d_x,%d_y,%d_z", i, i, i);
    fprintf(outputFile, "\n");

    walk_leaves(tree, set_leaf_pos);
    add_leaves_to_tree(tree);

    printf("Beginning threading...\n");
    clock_gettime(CLOCK_REALTIME, &time_start);
    calc_force(tree);
    apply_force(tree);
    apply_velocity(tree);
    clock_gettime(CLOCK_REALTIME, &time_stop);
    printf("Done threading\n");
    times[0] = interval(time_start, time_stop);

    for(int i = 0; i < ITERS; i++) {
        clock_gettime(CLOCK_REALTIME, &time_start);
        calc_center_of_mass(tree);
        rebalance(tree);
        clock_gettime(CLOCK_REALTIME, &time_stop);
        times[i + 1] = interval(time_start, time_stop);

        fprintf(outputFile, "%d", i);
        walk_leaves(tree, print_leaf_pos);
        fprintf(outputFile, "\n");

        #ifdef PRINT_CALCULATING
            if((i % 10) == 0) {
                printf("\rCalculating center of mass and rebalancing: %.2f%%", (((float) i * 100) / ITERS));
                fflush(stdout);
            }
        #endif

        }

    #ifdef PRINT_CALCULATING
        printf("\nDone!\n");
    #endif

    fclose(outputFile);
    destroy_tree(tree);

    clock_gettime(CLOCK_REALTIME, &tot_time_stop);
    double totalTime = interval(tot_time_start, tot_time_stop);

    double time = 0;
    for(int i = 0; i < ITERS; i++) {
        time += times[i];
    }

    printf("\n");
    printf("Calc  time: %.2f\n", time);
    printf("Total time: %.2f\n", totalTime);
    printf("Over  time: %.2f\n\n", totalTime - time);

    return 0;
}
