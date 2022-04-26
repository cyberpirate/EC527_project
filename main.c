
#include <stdio.h>
#include <bits/time.h>
#include <time.h>
#include "params.h"
#include "oct_tree.h"
#include "rand_gen.h"
#include "utils.h"
#include <pthread.h> // Multithreading

#define PRINT_CALCULATING

FILE* outputFile;

void print_leaf_pos(struct OctTree* tree, struct Leaf* leaf) {
    fprintf(outputFile, ",%f,%f,%f", leaf->pos.x, leaf->pos.y, leaf->pos.z);
}

void set_leaf_pos(struct OctTree* tree, struct Leaf* leaf) {
//    leaf->pos = rand_pos();
    leaf->pos = rand_torus(TORUS_OUTER, TORUS_INNER);
    leaf->velocity = vel_from_pos(&leaf->pos);
    mult_scalar(&leaf->velocity, TORUS_VEL/UNIVERSE_SIZE);
}

// Global tree structure for multithreading
struct OctTree* tree;

/*------------------------ Pthread functions ------------------------*/

void *calc_force_threaded(void *ptr)
{
    calc_force(tree);
    pthread_exit(NULL);
}

void *apply_force_threaded(void *ptr)
{
    apply_force(tree);
    pthread_exit(NULL);
}

void *apply_velocity_threaded(void *ptr)
{
    apply_velocity(tree);
    pthread_exit(NULL);
}

/*--------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    reset_rand();

    //struct OctTree* tree = create_tree(POINT_COUNT);
    tree = create_tree(POINT_COUNT);
    double times[ITERS];
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
//    fprintf(outputFile, "%d", 0);
//    walk_leaves(tree, print_leaf_pos);
//    fprintf(outputFile, "\n");
//    return 0;
    add_leaves_to_tree(tree);


#ifdef PRINT_CALCULATING
    printf("calculating: 0%%");
#endif
    pthread_t thread_id1[ITERS], thread_id2[ITERS], thread_id3[ITERS];

    for(int i = 0; i < ITERS; i++) {
        clock_gettime(CLOCK_REALTIME, &time_start);
        calc_center_of_mass(tree);
        //calc_force(tree);
        pthread_create(&thread_id1[i], NULL, calc_force_threaded, NULL);
        //apply_force(tree);
        pthread_create(&thread_id2[i], NULL, apply_force_threaded, NULL);
        //apply_velocity(tree);
        pthread_create(&thread_id3[i], NULL, apply_velocity_threaded, NULL);
        //pthread_join(thread_id1[i], NULL);
        //pthread_join(thread_id2[i], NULL);
        //pthread_join(thread_id3[i], NULL);
        rebalance(tree);
        clock_gettime(CLOCK_REALTIME, &time_stop);
        times[i] = interval(time_start, time_stop);

        fprintf(outputFile, "%d", i);
        //walk_leaves(tree, print_leaf_pos);
        fprintf(outputFile, "\n");

#ifdef PRINT_CALCULATING
        if((i % 10) == 0) {
            printf("\rcalculating: %.2f%%", (((float) i * 100) / ITERS));
            fflush(stdout);
        }
#endif
    }

    for (int j = 0; j < ITERS; j++)
    {
        pthread_join(thread_id1[j], NULL);
        pthread_join(thread_id2[j], NULL);
        pthread_join(thread_id3[j], NULL);
    }
/*
    for (int j = 0; j < ITERS; j++)
    {
        pthread_join(thread_id2[j], NULL);
    }

    for (int j = 0; j < ITERS; j++)
    {
        pthread_join(thread_id3[j], NULL);
    }
*/
#ifdef PRINT_CALCULATING
    printf("\rDone!\n");
#endif

    fclose(outputFile);
    destroy_tree(tree);

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
