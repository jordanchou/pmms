#include <stdio.h>
#include <stdlib.h>

void read_matrix(char* filename, int rows, int cols, int (*matrix)[rows][cols])
{
    FILE *in_file;
    int number;

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
            if (fscanf(in_file, "%d", &(*(matrix))[ii][jj]) != 1)
            {
                fprintf(stderr, "Error reading from file: %s\n", filename);
                fclose(in_file);
                exit(1);//If any occurs then end the program
            }
        }
    }
    fclose(in_file);//Close the file after reading
}

int main()
{
    int M, N;

    M = 3;
    N = 2;
    int matrix[M][N];

    read_matrix("matrix_a", M, N, &matrix);

    for (int ii = 0; ii < M; ii++)
    {
        for (int jj = 0; jj < N; jj++)
        {
            printf("MATRIX_ONE: %d\n", matrix[ii][jj]);
        }
    }
}