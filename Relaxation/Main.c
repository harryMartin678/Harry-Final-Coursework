#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define PERCISION 3
struct ThreadData{

	int threadNo;
	int start;
	int end;
};

clock_t timer;

typedef struct ThreadData ThreadData;

int NUM_THREADS;
int SIZE;

pthread_barrier_t calculateBarr;
pthread_barrier_t updateBarr;
pthread_barrier_t restartBarr;
pthread_mutex_t threadsDoneMutex;
pthread_t *threads;

int threadsDone;
double percisionNumber;

double **data;

double average(int x, int y);
double* calculateAverages(double* results,ThreadData* threadData,int* calState);
void UpdateData(double* results, ThreadData* threadData,int* calState,int calStateSize);
void calPercision(double old,double new,int* finished);
int power(int n, int p);
void *splitWork(void* arg);
void printArray();
void createData();


double average(int x, int y){

	double avg = 0.0;
	int control[8] = {1,0,0,1,-1,0,0,-1};

	int c;
	for(c = 0; c < 8; c+=2){

		avg += data[x + control[c]][y + control[c+1]];
	}

	return avg/4;
}

double* calculateAverages(double* results,ThreadData* threadData,int* calState){

	int r;
	for(r = threadData->start; r <= threadData->end; r++){

		int offset = r - threadData->start;
		int x = (r/(SIZE-2))+1;
		int y = (r%(SIZE-2))+1;

		if(*(calState + (sizeof(int)*offset))){

			*(results+((r - threadData->start) * sizeof(double))) = data[x][y];

		}else{

			*(results+((r - threadData->start) * sizeof(double))) = average(x,y);
		}
	}

	return results;
}

void UpdateData(double* results, ThreadData* threadData,int* calState,int calStateSize){

	*(calState + (sizeof(int) * calStateSize)) = 0;

	int d;
	for(d = threadData->start; d <= threadData->end; d++){



		int x = (d/(SIZE-2))+1;
		int y = (d%(SIZE-2))+1;
		int offset = d - threadData->start;
		if(!*(calState + (offset * sizeof(int)))){

			calPercision(data[x][y],*(results+(offset*sizeof(double))),
					(calState + (offset * sizeof(int))));

			data[x][y] = *(results+(offset*sizeof(double)));

			*(calState + (sizeof(int) * calStateSize)) += *(calState + (sizeof(int) * offset));
		}
		else{

			*(calState + (sizeof(int) * calStateSize)) += 1;
		}

	}
}

void calPercision(double old,double new,int* finished){

	double oldP = round(old * percisionNumber)/percisionNumber;
	double newP = round(new * percisionNumber)/percisionNumber;

	*finished = oldP == newP;

}

int power(int n, int p){

	int answer = 1;
	while(p > 0){

		answer = answer * n;
		p--;
	}

	return answer;
}

void *splitWork(void* arg){

	ThreadData* threadData = (ThreadData*)arg;
	int calStateSize = (threadData->end - threadData->start + 1);
	int* calState = (int*)malloc(sizeof(int)*calStateSize);
	double* results = (double*)malloc(sizeof(double)*(threadData->end - threadData->start+1));
	int endedCalculation = 0;

	int i;
	for(i = 0; i <= calStateSize; i++){

		*(calState + (sizeof(int)*i)) = 0;
	}

	while(1){

		if(endedCalculation == 0 && *(calState + (sizeof(int)*calStateSize)) == calStateSize){

			endedCalculation = 1;
			pthread_mutex_lock(&threadsDoneMutex);
			threadsDone++;
			pthread_mutex_unlock(&threadsDoneMutex);
		}

		if(!endedCalculation){
			calculateAverages(results,threadData,calState);

		}
		int error1 = pthread_barrier_wait(&calculateBarr);

		if(error1 == 0 || error1 == PTHREAD_BARRIER_SERIAL_THREAD){

			if(!endedCalculation){
				UpdateData(results,threadData,calState,calStateSize);
			}
			int error2 = pthread_barrier_wait(&updateBarr);

			if(error2 != 0 && error2 != PTHREAD_BARRIER_SERIAL_THREAD){

				printf("Update Barrier error\n");
				break;
			}else{

				if(threadsDone == NUM_THREADS){

					free(calState);
					free(results);
					break;
				}

				int error3 = pthread_barrier_wait(&restartBarr);

				if(error3 != 0 && error3 != PTHREAD_BARRIER_SERIAL_THREAD){

					printf("Restart Barrier error\n");
					break;
				}
			}
		}else{

			printf("Calculate Barrier error\n");
		}
	}

	return arg;
}

void printArray(){

	int x;
	int y;
	for(x =0; x < SIZE; x++){
		for(y = 0; y < SIZE; y++){

			printf("%f ",data[y][x]);
		}
		printf("\n");
	}
	printf("\n\n");
}

void createData(){

	data = (double**)malloc(sizeof(double*)*SIZE);
	srand(time(NULL));
	int x;
	int y;
	for(x = 0; x < SIZE; x++){

		data[x] = (double*)malloc(sizeof(double)*SIZE);
		for(y = 0; y < SIZE; y++){

			if(x == 0 || y == 0 || x == SIZE-1 || y == SIZE-1){

				data[x][y] = rand() % 2;

			}else{

				data[x][y] = 0.0;
			}
		}
	}

}


int main(int argc, char **argv) {



	if(argc < 3){

		printf("Too few arguments given. THREADS MATRIX_SIZE\n");
		return EXIT_FAILURE;

	}else if(argc > 3){

		printf("Too many arguments given. THREADS MATRIX_SIZE\n");
		return EXIT_FAILURE;
	}

	NUM_THREADS = atoi(argv[1]);
	SIZE = atoi(argv[2]);

	threadsDone = 0;
	createData();

	timer = clock();

	pthread_barrier_init(&calculateBarr,NULL,NUM_THREADS);
	pthread_barrier_init(&updateBarr,NULL,NUM_THREADS);
	pthread_barrier_init(&restartBarr,NULL,NUM_THREADS);
	pthread_mutex_init(&threadsDoneMutex,NULL);

	percisionNumber = power(10,PERCISION);

	threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
	printArray();
	int numPerThread = power(SIZE-2,2) / NUM_THREADS;
	int nextStartPt = 0;
	int t;
	for(t = 0; t < NUM_THREADS; t++){

	  ThreadData* data = (ThreadData*)malloc(sizeof(ThreadData));

	  if(t == NUM_THREADS-1){

		  data->start = nextStartPt;
		  data->end = power(SIZE-2,2)-1;


	  }else{

		  data->start = nextStartPt;
		  data->end = data->start + numPerThread;
		  nextStartPt = data->end + 1;
	  }
	  data->threadNo = t;
	  pthread_create(&threads[t],NULL,splitWork,data);
	}

	for(t = 0; t < NUM_THREADS; t++){

	  pthread_join(threads[t],NULL);
	}

	printArray();
	timer = clock() - timer;
	long milSec = timer * 1000.0 / CLOCKS_PER_SEC;
	printf("Time Taken(milliseconds): %ld %ld\n",milSec,timer);

	return EXIT_SUCCESS;
}
