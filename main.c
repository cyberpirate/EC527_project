
#include <stdio.h>
#include <bits/time.h>
#include <time.h>
#include "params.h"
#include "oct_tree.h"
#include "rand_gen.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    reset_rand();

    struct OctNode* root = create_oct_node();
    struct Leaf* leaves[POINT_COUNT];
    double times[ITERS];
    struct timespec time_start, time_stop;
    struct timespec tot_time_start, tot_time_stop;
    clock_gettime(CLOCK_REALTIME, &tot_time_start);

    FILE* outputFile = fopen("output.csv", "w");

    if(outputFile == nullptr) {
        printf("Error, can't open output.csv\n");
        return 1;
    }

    fprintf(outputFile, "frame");
    for(int i = 0; i < POINT_COUNT; i++)
        fprintf(outputFile, ",%d_x,%d_y,%d_z", i, i, i);
    fprintf(outputFile, "\n");


    for(int i = 0; i < POINT_COUNT; i++) {
        leaves[i] = create_leaf();
        leaves[i]->pos = rand_pos();
        add_leaf(root, leaves[i]);
    }



    for(int i = 0; i < ITERS; i++) {
        clock_gettime(CLOCK_REALTIME, &time_start);
        calc_center_of_mass(root);
        calc_force(root);
        apply_force(root);
        apply_velocity(root);
        rebalance(root);
        clock_gettime(CLOCK_REALTIME, &time_stop);
        times[i] = interval(time_start, time_stop);

        fprintf(outputFile, "%d", i);
        for(int j = 0; j < POINT_COUNT; j++)
            fprintf(outputFile, ",%f,%f,%f", leaves[j]->pos.x, leaves[j]->pos.y, leaves[j]->pos.z);
        fprintf(outputFile, "\n");
    }

    for(int i = 0; i < POINT_COUNT; i++) {
        remove_leaf(root, leaves[i]);
        destroy_leaf(leaves[i]);
    }

    fclose(outputFile);
    destroy_oct_node(root);

    clock_gettime(CLOCK_REALTIME, &tot_time_stop);
    double totalTime = interval(tot_time_start, tot_time_stop);

    double time = 0;
    for(int i = 0; i < ITERS; i++) {
        time += times[i];
    }
    printf("\n\n");
    printf("calc  time: %.2f\n", time);
    printf("total time: %.2f\n", totalTime);
    printf("over  time: %.2f\n", totalTime - time);

    return 0;
}
