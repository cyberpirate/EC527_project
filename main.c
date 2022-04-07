
#include <stdio.h>
#include <bits/time.h>
#include <time.h>
#include "params.h"
#include "oct_tree.h"
#include "rand_gen.h"

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

int main(int argc, char *argv[])
{
    reset_rand();

    struct OctNode* root = create_oct_node();
    struct Leaf* leaves[LEAF_COUNT];
    double times[ITERS];
    struct timespec time_start, time_stop;
    struct timespec tot_time_start, tot_time_stop;
    clock_gettime(CLOCK_REALTIME, &tot_time_start);


    printf("frame");
    for(int i = 0; i < LEAF_COUNT; i++)
        printf(",%d_x,%d_y,%d_z", i, i, i);
    printf("\n");


    for(int i = 0; i < LEAF_COUNT; i++) {
        leaves[i] = create_leaf();
        leaves[i]->pos = rand_pos();
        add_leaf(root, leaves[i]);
    }



    for(int i = 0; i < ITERS; i++) {
        clock_gettime(CLOCK_REALTIME, &time_start);
        calc_center_of_mass(root);
        calc_force(root);
        apply_force(root);
        rebalance(root);
        clock_gettime(CLOCK_REALTIME, &time_stop);
        times[i] = interval(time_start, time_stop);

        printf("%d", i);
        for(int j = 0; j < LEAF_COUNT; j++)
            printf(",%f,%f,%f", leaves[i]->pos.x, leaves[i]->pos.y, leaves[i]->pos.z);
        printf("\n");
    }

    for(int i = 0; i < LEAF_COUNT; i++) {
        remove_leaf(root, leaves[i]);
        destroy_leaf(leaves[i]);
    }

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
