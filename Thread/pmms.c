/*
FILE:pmms.c
AUTHOR:Jordan Pinglin Chou
USERNAME:18348691
UNIT: Operating Systems
PURPOSE: Multiplies 2 matrices and outputs the sum
REFERENCE:-
COMMENTS:-
REQUIRES:-
Last Mod: 08-05-2016
*/

#include "pmms.h"

//Have to keep them as global variables in order to share between threads
static pthread_mutex_t mutex;//mutex variable
static pthread_cond_t full;//condition variable for subtotal full/empty
int M, N, K;//matrix dimensions

int is_full, **matrix_a, **matrix_b, **matrix_c;
//pthread_t thread_id;
SubTotal sub_total;


int main(int argc, char **argv)
{
	int thread_num, **thread_args;
	pthread_t temp;

	check_usage(argc, argv);//Check the command line arguments

	M = atoi(argv[3]);//Get the number of rows of matrix a
	N = atoi(argv[4]);//Get the number of rows of matrix b
	K = atoi(argv[5]);//Get the number of columns of matrix b

	initialise_array(&matrix_a, M, N);//Initialise and allocate memory for
	initialise_array(&matrix_b, N, K);// all the matrices
	initialise_array(&matrix_c, M, K);

	initialise_array(&thread_args, M, 1);//Initialise and allocate memory for
                                        // argument array

	read_matrix(argv[1], M, N, matrix_a);//Read in matrix a
	read_matrix(argv[2], N, K, matrix_b);//Read in matrix b

	pthread_mutex_init(&mutex, NULL);//Initialise the mutex variable
	pthread_cond_init(&full, NULL);//Initialise the condition variable
	is_full = FALSE;//Initialise full to FALSE

    /**
     * Loop through the number of rows needed for the result matrix and
     * create a thread for each of them and pass the child handler to do their
     * calculations. The row number of the thread is given by an int pointer
     * contained within thread_args
     */
	for (int ii=0; ii<M; ii++)
	{
		thread_args[ii][0] = ii;//Set the row number

        //create the thread
		pthread_create(&temp, NULL, child_handler, thread_args[ii]);
	}
printf("HI\n");
    free_array(M, 1, thread_args);

	parent_handler();//Parent process goes into here
	cleanup();//Clean up memory and variables



}

/**
 * Handles the process for each child thread
 * @param  args int pointer pointing to the row number that child works on
 * @return      a void pointer to NULL because pthread_create wants it
 */
void *child_handler(void *args)
{
	int total, sum, num;

	num = *((int*)args);//extract the row number from the void pointer

	total = 0;//Initialise total to 0

    for (int ii = 0; ii<K; ii++)
    {
        sum = 0;

        for (int jj = 0; jj<N; jj++)
        {
            sum = sum + (matrix_a)[num][jj] * (matrix_b)[jj][ii];
        }

        (matrix_c)[num][ii] = sum;
    }
    for (int ii = 0; ii< K; ii++)
    {
        total = total + matrix_c[num][ii];
    }

	pthread_mutex_lock(&mutex);//acquire mutex lock

	while(is_full)//wait if the subtotal is currently full
	{
		pthread_cond_wait(&full, &mutex);//wait
	}

	sub_total.sum = total;//Not full so place total inside subtotal data struct.
	sub_total.tid = pthread_self();//place thread ID inside data structure

	is_full = TRUE;//set is_full to true

	pthread_mutex_unlock(&mutex);//release the mutex lock
	pthread_cond_broadcast(&full);//let other threads know that lock is avail.
    pthread_detach(pthread_self());//Release all resources
	return NULL;
}

/**
 * Handles the process required of the parent process.
 * Sums up the sub_totals calculated by the threads and outputs them.
 * Also outputs the sum.
 */
void parent_handler()
{
	int total;

	total = 0;

    /**
     * Loop through the number of rows and acquire the lock, while the sub_total
     * is empty it waits until the thread updates the subtotal. If the sub_total
     * is full it then outputs it and adds it to a sum
     */
	for (int ii=0; ii<M; ii++)
	{
		pthread_mutex_lock(&mutex);//acquire lock

		while (!is_full)//wait until thread places the subtotal in
		{
			pthread_cond_wait(&full, &mutex);
		}

		printf("Subtotal produced by thread with ID %lu: %d\n",
				sub_total.tid, sub_total.sum);

		total = total + sub_total.sum;//sum up the subtotal

		is_full = FALSE;//set is_full to false after consuming the subtotal

		pthread_mutex_unlock(&mutex);//release the lock
		pthread_cond_signal(&full);//signal that full is now empty
	}

	printf("Total: %d\n", total);
}

/**
 * Clean up the mutex and condition variables and free all the arrays for the
 * matrices
 */
void cleanup()
{
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&full);

	free_array(M, N, matrix_a);
	free_array(N, K, matrix_b);
	free_array(M, K, matrix_c);
}

/**
 * Frees a dynamically allocated array
 * @param rows   the number of rows the array has
 * @param cols   the number of columns the array has
 * @param matrix the array to free
 */
void free_array(int rows, int cols, int **matrix)
{
	for (int ii=0; ii<rows; ii++)
	{
		free(matrix[ii]);
	}

	free(matrix);
}

/**
 * Initialises a 2D array given the rows and columns (dimensions)
 * @param matrix A triple pointer (pointer to 2D array)
 * @param rows   Number of rows
 * @param cols   Number of columns
 */
void initialise_array(int ***matrix, int rows, int cols)
{
	*matrix = malloc(rows * sizeof(int*));

	for (int ii=0; ii< rows; ii++)//allocate memory for each row
	{
		(*matrix)[ii] = malloc(cols * sizeof(int));
	}
}

/**
 * Reads in a matrix into a 2D array from file
 * @param filename file to read matrix in from
 * @param rows     number of rows to read from the matrix
 * @param cols     number of cols to read from the matrix
 * @param matrix   the 2D array to read the file into
 */
void read_matrix(char* filename, int rows, int cols, int **matrix)
{
    FILE *in_file;
    int number;

    in_file = fopen(filename, "r");//Open the file for reading

    if (in_file == NULL)//If the file cannot be opened
    {
        fprintf(stderr, "Could not open file: \"%s\", does it exist?\n", filename);

        exit(1);//End the program
    }

    for (int ii=0; ii<rows; ii++)//Loop through the columns
    {
        for (int jj=0; jj<cols; jj++)//Loop through the rows
        {
            //Read the number from file
            if (fscanf(in_file, "%d", &(matrix)[ii][jj]) != 1)
            {
                fprintf(stderr, "Error reading from file: '%s', please check the format of the file\n", filename);
                fclose(in_file);
                exit(1);//If any occurs then end the program
            }
        }
    }

    fclose(in_file);//Close the file after reading
}


/**
 * Checks the number of command line arguments
 * @param argc the number of command line arguments
 * @param argv the command line arguments
 */
void check_usage(int argc, char** argv)
{
    if (argc != 6)
    {
        fprintf(stderr, "Usage: ./pmms matrix_a matrix_b M N K\n");
        exit(1);//If not enough command line args then exit
    }

    if (atoi(argv[3]) <= 0 || atoi(argv[4]) <= 0 || atoi(argv[5]) <= 0)
    {
        fprintf(stderr, "Dimensions must be greater than 0\n");
        exit(1);//If any dimensions are invalid then exit
    }
}
