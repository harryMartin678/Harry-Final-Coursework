/*
 * Author: Harry Martin (hm474)
 * Parallel Processing
 * Shared Memory Architecture Coursework
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

//ThreadData store data that each thread needs
struct ThreadData{

	int threadNo;
	int start;
	int end;
	int* threadsDone;
};

typedef struct ThreadData ThreadData;

//globals
int NUM_THREADS;
int SIZE;
double **data;
double **workingData;

//barriers
pthread_barrier_t calculateBarr;
pthread_barrier_t updateBarr;
pthread_barrier_t restartBarr;
//mutex
pthread_mutex_t threadsDoneMutex;

//function definitions
double average(int x, int y);
void calculateAverages(ThreadData* threadData);
int UpdateData(ThreadData* threadData);
int calPercision(double old,double new);
void *splitWork(void* arg);
void printArray();
double** createData();


//calculates the average of four neighbours. The two ints x and y are the position in the array
//that is to be averaged.
double average(int x, int y){

	double avg = 0.0;
	//represents the four neighbours
	int control[8] = {1,0,0,1,-1,0,0,-1};

	int c;
	for(c = 0; c < 8; c+=2){

		avg += data[x + control[c]][y + control[c+1]];
	}

	return avg/4;
}

//Calculates the averages of all the numbers that are allocated to this thread. The threads start and end
//positions are placed in the ThreadData* structure argument.
void calculateAverages(ThreadData* threadData){

	int r;
	for(r = threadData->start; r <= threadData->end; r++){

		//convert from the SIZE-2 by SIZE-2 coordinates to SIZE by SIZE coordinates
		int x = (r/(SIZE-2))+1;
		int y = (r%(SIZE-2))+1;

		//get the average
		double avg = average(x,y);

		//if the average is different to the last value by a greater value than the percision then change
		//otherwise do not
		//place the new value in the workingData array which will not interfere with the data array
		//that the threads are looking at to calculate their own averages
		if(!calPercision(avg,data[x][y])){

			workingData[x][y] = avg;

		}
	}
}

//updates the data array with the newer calculated values
int UpdateData(ThreadData* threadData){

	int d;
	int finished = 0;
	for(d = threadData->start; d <= threadData->end; d++){

		//convert from the SIZE-2 by SIZE-2 coordinates to SIZE by SIZE coordinates
		int x = (d/(SIZE-2))+1;
		int y = (d%(SIZE-2))+1;

		//update the data if there is a difference between the new and old value greater than the percision
		if(!calPercision(data[x][y],workingData[x][y])){

			data[x][y] = workingData[x][y];

		}else{

			//else record that this position in the array has finished calculating
			finished++;
		}

	}

	return finished;
}

//calculates if a value has reached the percision of 0.1
int calPercision(double old,double new){

	return fabs(new - old) < 0.1;

}

//calculates the averages of a sub-section of the array
void *splitWork(void* arg){

	ThreadData* threadData = (ThreadData*)arg;
	int finished = 0;
	int endedCalculation = 0;
	while(1){

		//the number of values who meet the percision requirement is the same as the number of values the thread
		//has been assigned then declare that this thread is done
		//do not declare that a thread is done more than once, endedCalculation ensures this
		if(endedCalculation == 0 && finished == (threadData->end - threadData->start)+1){

			endedCalculation = 1;
			//increment a shared variable
			pthread_mutex_lock(&threadsDoneMutex);
			(*threadData->threadsDone)++;
			pthread_mutex_unlock(&threadsDoneMutex);
		}

		//if the thread is not done then recalculate all averages
		if(!endedCalculation){
			calculateAverages(threadData);

		}

		//wait for all the threads to have calculated all of there averages
		int error1 = pthread_barrier_wait(&calculateBarr);

		//if there is no error
		if(error1 == 0 || error1 == PTHREAD_BARRIER_SERIAL_THREAD){

			//update the data array if the thread is not done
			if(!endedCalculation){
				finished = UpdateData(threadData);
			}

			//if all the threads are done then terminate
			//barriers mean that threads have to exit at the same time
			if(*(threadData->threadsDone) == NUM_THREADS){

				break;
			}

			//wait for every thread to update before moving recalculating the averages again
			int error3 = pthread_barrier_wait(&restartBarr);

			if(error3 != 0 && error3 != PTHREAD_BARRIER_SERIAL_THREAD){

				printf("Restart Barrier error\n");
				break;
			}
		}else{

			printf("Calculate Barrier error\n");
		}
	}

	return arg;
}

//prints the data array
void printArray(int SIZE){

	int end = SIZE;
	int start = 0;

	//print the center only if the array is greater than 20 by 20
	if(SIZE > 20){

		start = (SIZE/2) - 5;
		end = (SIZE/2) + 5;
	}
	int x;
	int y;
	for(x =start; x < end; x++){
		for(y = start; y < end; y++){

			printf("%f ",data[y][x]);
		}
		printf("\n");
	}
	printf("\n\n");
}

//creates an array of size SIZE
//creates the same array each time
double** createData(){

	double** data = (double**)malloc(sizeof(double*)*SIZE);
	int x;
	int y;
	for(x = 0; x < SIZE; x++){

		data[x] = (double*)malloc(sizeof(double)*SIZE);
		for(y = 0; y < SIZE; y++){

			//the boundaries are all 1.0s
			if(x == 0 || y == 0 || x == SIZE-1 || y == SIZE-1){

				data[x][y] = 1.0;

			}else{

				//the inside of the array is 1.0 or 0.0s
				if((x + y) % 4 == 0 || (x + y) % 7 == 0){

					data[x][y] = 1.0;
				}else{
					data[x][y] = 0.0;
				}

			}
		}
	}

	return data;

}

//frees an multi-dimensional array
void freeData(double** data){

	int a;
	for(a = 0; a < SIZE; a++){

		free(data[a]);
	}

	free(data);
}


int main(int argc, char **argv) {

	//error messages in case of invalid command line arguments
	if(argc < 3){

		printf("Too few arguments given. THREADS MATRIX_SIZE\n");
		return EXIT_FAILURE;

	}else if(argc > 3){

		printf("Too many arguments given. THREADS MATRIX_SIZE\n");
		return EXIT_FAILURE;

	}

	pthread_t *threads;

	//obtain the number of threads and the size of the array from the command line arguments
	NUM_THREADS = atoi(argv[1]);
	SIZE = atoi(argv[2]);

	//the number of array elements that need to be shared out
	int elementsToAssign = (int)pow(SIZE-2,2);

	//if the number of threads is greater than the elements to assign then it is impossible
	//to share out the elements in the array to each of these threads
	if(NUM_THREADS >= elementsToAssign){

		printf("TOO MANY THREADS. THREADS must be < (MATRIX_SIZE-2) ^ 2.\n");
		return EXIT_FAILURE;
	}

	//get the time
	struct timeval start, end;
	gettimeofday(&start, NULL);

	//threadsDone indicates whether all the threads have started
	//threadsDone is intialized
	int* threadsDone = (int*)malloc(sizeof(int));
	*threadsDone = 0;
	//create the data and workingData arrays
	data = createData();
	workingData = createData();

	//set up the barriers
	pthread_barrier_init(&calculateBarr,NULL,NUM_THREADS);
	pthread_barrier_init(&updateBarr,NULL,NUM_THREADS);
	pthread_barrier_init(&restartBarr,NULL,NUM_THREADS);
	//set up the mutex for the threads Done shared variable
	pthread_mutex_init(&threadsDoneMutex,NULL);

	//assign the thread array
	threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
	//print the data array's initial state
	printArray(SIZE);
	//calculate how many array elements are going to be assigned to each thread
	double numPerThread = (double)elementsToAssign / (double)NUM_THREADS;

	//make sure that the num per thread is rounded down if for each thread has less than a row
	if(NUM_THREADS > SIZE){

		numPerThread--;
	}

	int nextStartPt = 0;
	int t;
	//create all of the threads
	for(t = 0; t < NUM_THREADS; t++){

	  ThreadData* data = (ThreadData*)malloc(sizeof(ThreadData));

	  //assign the rest of the elements
	  if(t == NUM_THREADS-1){

		  data->start = nextStartPt;
		  data->end = elementsToAssign-1;

      //otherwise assign numPerThread number of elements
	  }else{

		  data->start = nextStartPt;
		  data->end = data->start + numPerThread;
		  nextStartPt = data->end + 1;
	  }


	  //give the thread data struct the threadsDone shared variable
	  data->threadNo = t;
	  data->threadsDone = threadsDone;
	  //if this is a sequential program do not bother creating a seperate thread
	  if(NUM_THREADS == 1){

		  splitWork(data);

	  }else{

		  //otherwise create the new thread
		  pthread_create(&threads[t],NULL,splitWork,data);
	  }



	}

	//if this program is not sequential then wait for all of the threads to finish
	if(NUM_THREADS > 1){
		for(t = 0; t < NUM_THREADS; t++){

		  pthread_join(threads[t],NULL);
		}
	}

	//print the array after relaxation
	printArray(SIZE);
	//get the end time of the program
	gettimeofday(&end, NULL);
	//free both arrays
	freeData(data);
	freeData(workingData);
	//print the time taken for the program to relax the array
	printf("Time Taken(seconds): %f\n",((end.tv_sec * 1000000 + end.tv_usec)
			  - (start.tv_sec * 1000000 + start.tv_usec))/1000000.0);

	return EXIT_SUCCESS;
}
