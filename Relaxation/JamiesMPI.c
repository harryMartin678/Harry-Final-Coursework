#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>



double** mArray;
double** workArray;


int is_difference(int dimen, double prec)
{
    int i, j;
    double difference;
    int is_difference = 0; // If there is a difference, we need to keep working
    for (i = 0; i < dimen; i++)
    {
        for (j = 0; j < dimen; j++)
        {
            //find difference via absolute value of (original number - new number)
            difference = fabs(mArray[i][j] - workArray[i][j]);
            if (difference > prec)
            {
                is_difference = 1;
                //printf("Difference > precision found in pos %d, %d. Value: %f\n", i, j, difference);
            }
        }
    }
    return is_difference;
}

int bcastDone(int done)
{
    MPI_Bcast(&done, 1, MPI_INT, 0, MPI_COMM_WORLD);
    return done;
}


void copyArray_p(int num, double ***source, double ***dest)
{
    int i, j;
    for (i = 0; i < num; i++){
        for (j = 0; j < num; j++){
            (*dest)[i][j] = (*source)[i][j];
        }
    }
}


double **allocMemory(int dimension) {
    double *contBuf = (double *)malloc(dimension*dimension*sizeof(double));
    double **contArray = (double **)malloc(dimension*sizeof(double*));
    // As we send in contiguous blocks of memory, we allocate contiguously
    // such that we send one large chunk per MPI_Send/Bcast

    int i;
    for (i = 0; i < dimension; i++)
    {
        contArray[i] = &(contBuf[dimension*i]);
    }

    return contArray;
}

void printArray_p(int num, double ***printArray)
{
    int i, j;
    for (i = 0; i < num; i++){
        for (j = 0; j < num; j++){
            printf("%f ", (*printArray)[i][j]);
        }
    printf("\n");
    }
}

void master_io(int threads, int dimension)
{
    MPI_Status status;
    /*
    mArray[1][1] = 2;
    mArray[1][2] = 3;
    mArray[1][3] = 4;
    mArray[2][1] = 5;
    mArray[2][2] = 6;
    mArray[2][3] = 7;
    mArray[3][1] = 8;
    mArray[3][2] = 9;
    mArray[3][3] = 10;
    */
    int thr, rc;
    int totalAverages = ((dimension-2)*(dimension-2));
    double recvArray[totalAverages];
    //printArray_p(dimension, &mArray);
    //printf("Master threadID #%d threads number: \n", threads);
    //bcastDone(done); // Broadcast
    MPI_Bcast(&(mArray[0][0]), dimension*dimension, MPI_DOUBLE, 0, MPI_COMM_WORLD); // Broadcast initial array to all slaves

    //rc = MPI_Comm_size(MPI_COMM_WORLD, &threads);

    for(thr = 1; thr <= threads; thr++) //Start at 1 to exclude master process
    {
      int end = 0;
      int start = 0;
      int carry = 0;
      int recvIndex = 0;

      int totalAverages = ((dimension-2)*(dimension-2));
      int perThread = (totalAverages / threads);
      int remainderAverages = ((dimension-2)*(dimension-2)) % threads;
      // Deal with remainder; add one average to each threadid below/eq to remainder
      if (thr <= remainderAverages)
      {
        perThread++;
      }
      else // Update start and end positions for each non-remainder thread
      {
        start = start+remainderAverages;
        carry = remainderAverages;
      }
      start = start + ((thr-1)*perThread);
      end = carry + ((thr*perThread)-1);

      // Receive each temp array from slave processes
      rc = MPI_Recv(&recvArray, perThread, MPI_DOUBLE, thr, 11, MPI_COMM_WORLD, &status);

     // printf("Master threadID #%d has returned:\n", thr);
      if (perThread == 0)
      {
    //  printf("Nothing\n");
      }
      else
      {
    	int i;
        for (i = start; i <= end; i++)
        {
            int w = (i % (dimension-2))+1;
            int h = (i / (dimension-2))+1;
        //  printf("FThread #%d: Start %d, End %d, h %d, w %d \n", thr, start, end, h, w);
            workArray[h][w] = recvArray[recvIndex];
                //printArray(workArray);
            recvIndex++; //Increment through the received array
        }
      }
    }
}

int main(int argc, char *argv[])
{
    //Argument format: ./relax (dimension) (precision)

    int dimension = atoi(argv[1]);
    double precision = atof(argv[2]);
    int rc, threads, threadID, done;
    MPI_Init(&argc, &argv);
    rc = MPI_Comm_rank(MPI_COMM_WORLD, &threadID);
    rc = MPI_Comm_size(MPI_COMM_WORLD, &threads);
    done = 0;
    threads = threads-1;
    //printf("Thread count: %d\n", threads);

    //Allocate memory for main and working array
    mArray = allocMemory(dimension);
    workArray = allocMemory(dimension);

    if (threadID == 0)
    {
        //Initialize arrays
    	int i;
    	int j;
        for (i = 0; i < dimension; i++){
            for (j = 0; j < dimension; j++){
                if (i == 0 || j == 0 || i == (dimension-1) || j == (dimension-1))
                {
                    mArray[i][j] = 1;
                    workArray[i][j] = 1;
                }
                else
                {
                    mArray[i][j] = 0;
                    workArray[i][j] = 0;
                }
            }
        }
        bcastDone(done); // Initial call for done before we start
        master_io(threads, dimension); // Get the ball rolling
        while (is_difference(dimension, precision) == 1)
        {
            //printf("Difference still spotted, re-rolling ball..\n");
            //printArray_p(dimension, &workArray);
            copyArray_p(dimension, &workArray, &mArray);
            bcastDone(done); // Still not done
            master_io(threads, dimension);
        }

        done = 1;

        bcastDone(done); // Let slave threads know we're done
        printf("Printed final %d x %d array utilizing %d slave cores below.\n", dimension, dimension, threads);
        printArray_p(dimension, &workArray);

    }
    else
    {
        //rc = MPI_Recv(&done, 1, MPI_INT, 0, 10, MPI_COMM_WORLD, &status);
        while (bcastDone(done) == 0)
        {
            MPI_Bcast(&(mArray[0][0]), dimension*dimension, MPI_DOUBLE, 0, MPI_COMM_WORLD); // Receive main array from master process
            //rc = MPI_Recv(&(mArray[0][0]), dimension*dimension, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD, &status);
            int end = 0;
            int start = 0;
            int carry = 0;

            int totalAverages = ((dimension-2)*(dimension-2));
            int perThread = (totalAverages / threads);
            int remainderAverages = ((dimension-2)*(dimension-2)) % threads;
            // Deal with remainder; add one average to each threadid below/eq to remainder
            if (threadID <= remainderAverages)
            {
                perThread++;
            //  printf("Thread #%d: incremented perThread to %d\n", threadID, perThread);
            }
            else // Update start and end positions for each non-remainder thread
            {
                start = start+remainderAverages;
                carry = remainderAverages;
            }
            start = start + ((threadID-1)*perThread);
            end = carry + ((threadID*perThread)-1);
            int workIndex=0;
            double temp[perThread];
            //printf("Processor #%d: Start: %d, End: %d, Per Thread: %d :\n", threadID, start, end, perThread);
            if (perThread > 0)
            {
            //  printf("Thread #%d: Averages per thread: %d Start %d End %d\n", threadID, perThread, start, end);
                int i;
            	for (i = start; i <= end; i++)
                {
                    //Convert 1d coordinates to 2d coordinates
                    int w = (i % (dimension-2))+1;
                    int h = (i / (dimension-2))+1;
                    double n1 = mArray[h-1][w];
                    double n2 = mArray[h][w-1];
                    double n3 = mArray[h+1][w];
                    double n4 = mArray[h][w+1];
                    double mean = ((n1+n2+n3+n4)/4);
                //  printf("Processor #%d: h: %d, w: %d, Working on number: %f :, Mean: %f\n", threadID, h, w, mArray[h][w], mean);
                    temp[workIndex] = mean;
                    workIndex++;

                }
                //printf("perThread: %d, ", perThread);
                //printf("workIndex: %d\n", workIndex);
                rc = MPI_Send(&temp, perThread, MPI_DOUBLE, 0, 11, MPI_COMM_WORLD);
            }
            else{
                // If we have no operations to do, send empty temp array back
                workIndex = 0;
                temp[workIndex] = 0;
                rc = MPI_Send(&temp, perThread, MPI_DOUBLE, 0, 11, MPI_COMM_WORLD);
            }
        }
    }
    MPI_Finalize();
    free(mArray[0]);
    free(mArray);
    free(workArray[0]);
    free(workArray);
}
