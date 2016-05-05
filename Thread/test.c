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


int M, N, K;//matrix dimensions

int is_full, **matrix_a, **matrix_b, **matrix_c;

void read_matrix(char* filename, int rows, int cols, int **matrix)
{
    FILE *in_file;

    in_file = fopen(filename, "r");//Open the file for reading

    if (in_file == NULL)//If the file cannot be opened
    {
        fprintf(stderr, "Could not open file: %s\n", filename);

        exit(1);//End the program
    }

    for (int ii=0; ii<rows; ii++)//Loop through the columns
    {
        for (int jj=0; jj<cols; jj++)//Loop through the rows
        {
            //Read the number from file
            if (fscanf(in_file, "%d", &(matrix)[ii][jj]) != 1)
            {
                fprintf(stderr, "Error reading from file: %s\n", filename);
                fclose(in_file);
                exit(1);//If any occurs then end the program
            }

printf("MATRIX CELL:%d\n", (matrix)[ii][jj]);
        }
    }

    fclose(in_file);//Close the file after reading
}

void initialise_array(int ***matrix, int rows, int cols)
{
	*matrix = malloc(rows * sizeof(int*));

	for (int ii=0; ii< rows; ii++)//allocate memory for each row
	{
		(*matrix)[ii] = malloc(cols * sizeof(int));
	}
}

int main(int argc, char **argv)
{
    M = atoi(argv[3]);//Get the number of rows of matrix a
    N = atoi(argv[4]);//Get the number of rows of matrix b
    K = atoi(argv[5]);//Get the number of columns of matrix b

    initialise_array(&matrix_a, M, N);//Initialise and allocate memory for
    initialise_array(&matrix_b, N, K);// all the matrices
    initialise_array(&matrix_c, M, K);

    read_matrix(argv[1], M, N, matrix_a);//Read in matrix a
    read_matrix(argv[2], N, K, matrix_b);//Read in matrix b
}
