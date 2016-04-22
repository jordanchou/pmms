/*
FILE:linked_list.c
AUTHOR:Jordan Chou
USERNAME:18348691
UNIT: UCP
PURPOSE: Provides functions that can manipulate a linked list
REFERENCE:-
COMMENTS:-
REQUIRES:-
Last Mod:26-10-2015
*/

#ifndef PMMS_H
#define PMMS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

typedef struct SubTotal
{
    int sum;
    pid_t pid;
} SubTotal;

void read_matrix(char* filename, int rows, int cols, int (*matrix)[rows][cols]);
//void initialise_array(int rows, int cols, int (*matrix)[rows][cols]);
void check_usage(int argc, char** argv);
void set_memory(int *shm_matrix_one, int *shm_matrix_two, int *shm_matrix_three, 
                int *shm_semaphores, int *shm_sub_total, int M, int N, int K);
void map_blocks(int M, int N, int K, int (**matrix_one)[M][N], 
                int (**matrix_two)[N][K], int (**matrix_three)[M][K], 
                SubTotal **sub_total, sem_t **semaphores, int *shm_matrix_one,
                int *shm_matrix_two, int *shm_matrix_three, int *shm_sub_total
                , int *shm_semaphores);
void child_handler(int num, int M, int N, int K, int (*matrix_one)[M][N], 
                   int (*matrix_two)[N][K], int (*matrix_three)[M][K], sem_t *semaphores, 
                   SubTotal *sub_total);
void parent_handler(int M, int N, int K, int (*matrix_three)[M][K], sem_t *semaphores,
                    SubTotal *sub_total);
void cleanup(int M, int N, int K, int (*matrix_one)[M][N], int (*matrix_two)[N][K], int (*matrix)[M][K], 
             int shm_matrix_one, int shm_matrix_two, int shm_matrix_three,
             int shm_sub_total, int shm_semaphores,
             sem_t *semaphores, SubTotal *sub_total);
void mem_cleanup(char *shm_name, int shm, void *mem_id, int size);
#endif
