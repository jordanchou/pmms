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
#include <pthread.h>

#define TRUE 1
#define FALSE !TRUE

typedef struct SubTotal
{
    int sum;
    pthread_t tid;
} SubTotal;

void *child_handler(void *args);
void parent_handler();
void cleanup();
void free_array(int rows, int cols, int **matrix);
void initialise_array(int** matrix, int rows, int cols);
void read_matrix(char* filename, int rows, int cols, int **matrix);
void check_usage(int argc, char** argv);

#endif
