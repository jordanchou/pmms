/*
FILE:pmms.c
AUTHOR:Jordan Pinglin Chou
USERNAME:18348691
UNIT: Operating Systems
PURPOSE: Multiplies 2 matrices and outputs the sum
REFERENCE:-
COMMENTS:-
REQUIRES:-
Last Mod:19-04-2016
*/

#include "pmms.h"

/**
 * [main description]
 * @param  argc The number of command line arguments
 * @param  argv The command line arguments
 * @return      Exit condition
 */
int main(int argc, char** argv)
{
    int M, N, K, process_num, pid;
    int shm_sub_total, shm_semaphores, shm_matrix_one, shm_matrix_two,
        shm_matrix_three;
    sem_t sem_mutex, sem_full, sem_empty, *semaphores;
    SubTotal *sub_total;

    check_usage(argc, argv);//Check the command line arguments

    M = atoi(argv[3]);//Obtain M from command line arg
    N = atoi(argv[4]);//Obtain N from command line arg
    K = atoi(argv[5]);//Obtain K from command line arg

    //Set the memory needed for shared memory
    set_memory(&shm_matrix_one, &shm_matrix_two, &shm_matrix_three,
               &shm_semaphores, &shm_sub_total, M, N, K);

    //Declare the matrices
    int (*matrix_one)[M][N], (*matrix_two)[N][K], (*matrix_three)[M][K];


    //Map the SHM blocks
    map_blocks(M, N, K, &matrix_one, &matrix_two, &matrix_three,
                &sub_total, &semaphores, &shm_matrix_one, &shm_matrix_two,
                &shm_matrix_three, &shm_sub_total, &shm_semaphores);

    read_matrix(argv[1], M, N, matrix_one);//Read in the first matrix
    read_matrix(argv[2], N, K, matrix_two);//Read in second matrix

    //Initialise semaphores
    sem_init(&sem_mutex, 1, 1);
    sem_init(&sem_full, 1, 0);
    sem_init(&sem_empty, 1, 1);

    //Put semaphores in array (which is in memory block)
    semaphores[0] = sem_mutex;
    semaphores[1] = sem_full;
    semaphores[2] = sem_empty;

    //Create child processes
    process_num = 0;
    pid = -1;

    //Creat M children
    while (process_num < M && pid != 0)
    {
        pid = fork();//Create child
        process_num++;//Incrememnt child number
    }
    //Child process will break out of loop as pid != 0

    if (pid == 0)//Successful child process creation
    {
        child_handler(process_num, M, N, K, matrix_one, matrix_two, matrix_three, semaphores, sub_total);
    }
    else if (pid > 0)//Parent process
    {
        parent_handler(M, N, K, matrix_three, semaphores, sub_total);

        //Clean up as program is completed
        cleanup(M, N, K, matrix_one, matrix_two, matrix_three, shm_matrix_one,
                shm_matrix_two, shm_matrix_three, shm_sub_total, shm_semaphores,
                semaphores, sub_total);
    }
    else//Unsuccessful child process creation
    {
        //Clean up child creation failure
        fprintf(stderr, "Unable to create child: please run \"killall pmms\"\n");
        exit(1);
    }

    return 0;
}

/**
 * Maps shared memory to addresses
 * @param M                 the M dimension of matrix A
 * @param N                 the N dimension of matrix A and B
 * @param K                 the K dimension of matrix B
 * @param matrix_one        matrix A
 * @param matrix_two        matrix B
 * @param matrix_three      matrix C (result matrix)
 * @param sub_total         the subtotal data structure to store sum and pid
 * @param semaphores        the array of semaphores
 * @param shm_matrix_one    file descriptor for matrix A shared memory
 * @param shm_matrix_two    file descriptor for matrix B shared memory
 * @param shm_matrix_three  file descriptor for matrix C shared memory
 * @param shm_sub_total     file descriptor for sub_total shared memory
 * @param shm_semaphores    file descriptor for semaphores shared memory
 */
void map_blocks(int M, int N, int K, int (**matrix_one)[M][N],
                int (**matrix_two)[N][K], int (**matrix_three)[M][K],
                SubTotal **sub_total, sem_t **semaphores, int *shm_matrix_one,
                int *shm_matrix_two, int *shm_matrix_three, int *shm_sub_total
                , int *shm_semaphores)
{
    *matrix_one = mmap(NULL, M * N * sizeof(int), PROT_READ | PROT_WRITE,
                        MAP_SHARED, *shm_matrix_one, 0);
    *matrix_two = mmap(NULL, N * K * sizeof(int), PROT_READ | PROT_WRITE,
                        MAP_SHARED, *shm_matrix_two, 0);
    *matrix_three = mmap(NULL, M * K * sizeof(int), PROT_READ | PROT_WRITE,
                        MAP_SHARED, *shm_matrix_three, 0);
    *sub_total = mmap(NULL, sizeof(SubTotal), PROT_READ | PROT_WRITE,
                        MAP_SHARED, *shm_sub_total, 0);
    *semaphores = mmap(NULL, 3 * sizeof(sem_t), PROT_READ | PROT_WRITE,
                        MAP_SHARED, *shm_semaphores, 0);

    //If any mapping failed then exit
    if (*matrix_one == MAP_FAILED || *matrix_two == MAP_FAILED ||
        *matrix_three == MAP_FAILED || *sub_total == MAP_FAILED ||
        *semaphores == MAP_FAILED)
    {
        fprintf(stderr, "Failed to map memory");
        exit(1);
    }
}

/**
 * Creates and sets the required shared memory objects.
 * @param shm_matrix_one   file descriptor for matrix A
 * @param shm_matrix_two   file descriptor for matrix B
 * @param shm_matrix_three file descriptor for matrix C
 * @param shm_semaphores   file descriptor for semaphores
 * @param shm_sub_total    file descriptor for sub_total
 * @param M                the M dimension for matrix A
 * @param N                the N dimension for matrix A and B
 * @param K                the K dimension for matrix B
 */
void set_memory(int *shm_matrix_one, int *shm_matrix_two, int *shm_matrix_three,
                int *shm_semaphores, int *shm_sub_total, int M, int N, int K)
{
    *shm_matrix_one = shm_open("matrix_one", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    *shm_matrix_two = shm_open("matrix_two", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    *shm_matrix_three = shm_open("matrix_three", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    *shm_sub_total = shm_open("sub_total", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    *shm_semaphores = shm_open("semaphores", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    //Check that all the memory blocks were created correctly
    if (*shm_matrix_one == -1 || *shm_matrix_two == -1 ||
        *shm_matrix_three == -1 || *shm_sub_total == -1 || *shm_semaphores == -1)
    {
        fprintf(stderr, "Error creating shared memory");
        exit(1);
    }

    //Set the sizes for the shared memory blocks
    if (ftruncate(*shm_matrix_one, M * N * sizeof(int)) == -1)
    {
        fprintf(stderr,"Could not set size for matrix_one");
        exit(1);
    }

    if (ftruncate(*shm_matrix_two, N * K * sizeof(int)) == -1)
    {
        fprintf(stderr,"Could not set size for matrix_two");
        exit(1);
    }

    if (ftruncate(*shm_matrix_three, M * K * sizeof(int)) == -1)
    {
        fprintf(stderr,"Could not set size for matrix_three");
        exit(1);
    }

    if (ftruncate(*shm_sub_total, 1 * sizeof(SubTotal)) == -1)
    {
        fprintf(stderr,"Could not set size for sub_total");
        exit(1);
    }

    if (ftruncate(*shm_semaphores, 3 * sizeof(sem_t)) == -1)
    {
        fprintf(stderr,"Could not set size for semaphores");
        exit(1);
    }
}


/**
 * Routine for child processes. Calculates the required row (given by num) and
 * places the result in the sub_total data structure.
 * @param num          the process number for child process
 * @param M            the M dimension for matrix A
 * @param N            the N dimension for matrix A and B
 * @param K            the K dimension for matrix B
 * @param matrix_one   2D array representing matrix A
 * @param matrix_two   2D array representing matrix B
 * @param matrix_three 2D array representing matric C
 * @param semaphores   array of semaphores
 * @param sub_total    the subtotal data structure to store the subtotal + pid
 */
void child_handler(int num, int M, int N, int K, int (*matrix_one)[M][N],
                   int (*matrix_two)[N][K], int (*matrix_three)[M][K], sem_t *semaphores,
                   SubTotal *sub_total)
{
    int total, subtotal;
    num--;

    total = 0;

    //calc matrix C line
    for (int ii = 0; ii<K; ii++)
    {
        subtotal = 0;

        for (int jj = 0; jj<N; jj++)
        {
            subtotal = subtotal + (*matrix_one)[num][jj] * (*matrix_two)[jj][ii];
        }
printf("SUBTOTAL AT THIS POINT- ROW:%d:COLS:%d:%d\n", num, ii, subtotal);
        (*matrix_three)[num][ii] = subtotal;
    }

    //SHOULD BE LOOPING HERE TO CALCULATE THE TOTAL
    for (int ii = 0; ii < K; ii++)
    {
        total = total + (*matrix_three)[num][ii];
    }

    //acquire locks
    sem_wait(&semaphores[2]);//Empty lock
    sem_wait(&semaphores[0]);//Mutex lock


    //put into subtotal along with PID
    sub_total->sum = total;
    sub_total->pid = getpid();

    //release locks
    sem_post(&semaphores[0]);
    sem_post(&semaphores[1]);

}

/**
 * Routine for parent process. Totals the sub_totals generated by child
 * processes together and outputs them to console.
 * @param M            number of rows in final matrix (matrix C)
 * @param N            N dimension of matrix A
 * @param K            number of columns in final matrix (matrix C)
 * @param matrix_three the final (result) matrix
 * @param semaphores   array of semaphores
 * @param sub_total    data structure where subtotals and PID's are stored
 */
void parent_handler(int M, int N, int K, int (*matrix_three)[M][K], sem_t *semaphores,
                    SubTotal *sub_total)
{
    int total;

    total = 0;

    for (int ii=0; ii < M;ii++)
    {
        //wait
        sem_wait(&semaphores[1]);//Lock full
        sem_wait(&semaphores[0]);//Lock mutex

        printf("Subtotal produced by process with ID %d: %d\n",
                sub_total-> pid, sub_total->sum);

        total = total + sub_total->sum;

        sem_post(&semaphores[0]);//Unlock mutex
        sem_post(&semaphores[2]);//Unlock empty
    }

    printf("Total:%d\n", total);
}

/**
 * Cleans up all of the semaphores and shared memory blocks that were allocated
 * @param M              the M dimension of matrix A
 * @param N              the N dimension of matrix A
 * @param K              the K dimension of matrix B
 * @param matrix_one     2D array representing matrix A
 * @param matrix_two     2D array representing matrix B
 * @param matrix_three   2D array representing matrix C
 * @param shm_sub_total  file descriptor for the sub_total data structure
 * @param shm_semaphores file descriptor for semaphore array
 * @param semaphores     array of semaphores
 * @param sub_total      data structure that held results and PID's
 */
void cleanup(int M, int N, int K, int (*matrix_one)[M][N], int (*matrix_two)[N][K], int (*matrix_three)[M][K],
             int shm_matrix_one, int shm_matrix_two, int shm_matrix_three,
             int shm_sub_total, int shm_semaphores,
             sem_t *semaphores, SubTotal *sub_total)
{
    //Clean up semaphores
    sem_close(&(semaphores[0]));
    sem_close(&(semaphores[1]));
    sem_close(&(semaphores[2]));

    sem_destroy(&(semaphores[0]));
    sem_destroy(&(semaphores[1]));
    sem_destroy(&(semaphores[2]));

    //Free shared memory
    mem_cleanup("matrix_one", shm_matrix_one, matrix_one, M * N * sizeof(int));
    mem_cleanup("matrix_two", shm_matrix_two, matrix_two, N * K * sizeof(int));
    mem_cleanup("matrix_three", shm_matrix_three, matrix_three, M * K * sizeof(int));

    mem_cleanup("semaphores", shm_semaphores, semaphores, 3 * sizeof(sem_t));
    mem_cleanup("sub_total", shm_sub_total, sub_total, 1 * sizeof(SubTotal));
}

/**
 * Unmaps, unlinks and closes a shared memory object
 * @param shm_name name of the shared memory object
 * @param shm      the file descriptor
 * @param mem_id   the variable the memory was assigned to
 * @param size     the size of the shared memory object
 */
void mem_cleanup(char *shm_name, int shm, void *mem_id, int size)
{
    shm_unlink(shm_name);//unlink the shared memory object
    close(shm);//close the shared memory
    munmap(mem_id, size);//unmap the shared memory
}



/**
 * Reads in a matrix into a 2D array from file
 * @param filename file to read matrix in from
 * @param rows     number of rows to read from the matrix
 * @param cols     number of cols to read from the matrix
 * @param matrix   the 2D array to read the file into
 */
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
