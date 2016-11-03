#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 5
#define SIZE 5
struct ThreadData{

	int threadNo;
	int start;
	int end;
};

typedef struct ThreadData ThreadData;

pthread_barrier_t calculateBarr;
pthread_barrier_t updateBarr;

double data[SIZE][SIZE] = {
   {0.0, 1.0, 2.0, 3.0,4.0},
   {4.0, 5.0, 6.0, 7.0,6.0},
   {8.0, 9.0, 10.0, 11.0,10.0},
   {2.0, 3.0, 4.0, 1.0,0.0},
   {2.0, 3.0, 4.0, 2.0,4.0}
};

double average(int y, int x){

	double avg = 0.0;
	int control[8] = {1,0,0,1,-1,0,0,-1};

	int c;
	for(c = 0; c < 8; c+=2){

		printf("Avg: %d %d %d %d %f\n",y,x,y + control[c],x + control[c+1],
				data[y + control[c]][x + control[c+1]]);
		avg += data[y + control[c]][x + control[c+1]];
	}

	printf("Avg Is: %d %d %f\n",y,x,avg/4);
	return avg/4;
}

double* calculateAverages(ThreadData* threadData){

	double* results = (double*)malloc(sizeof(double)*(threadData->end - threadData->start));
	int r;
	for(r = threadData->start; r < threadData->end; r++){

		printf("Indices: %d y:%d x:%d\n",r,r/SIZE,r%SIZE);
		if(r % SIZE == 0){


			*(results+((r - threadData->start) * sizeof(double))) = data[r/SIZE][r%SIZE];

		}else{

			*(results+((r - threadData->start) * sizeof(double))) = average(r/SIZE,r%SIZE);
		}
	}

	return results;
}

void UpdateData(double* results, ThreadData* threadData){


}

void *splitWork(void* arg){

	ThreadData* threadData = (ThreadData*)arg;
	printf("thread: %d %d\n",threadData->start,threadData->end);
	double* results = calculateAverages(threadData);
	int res = pthread_barrier_wait(&calculateBarr);

	if(res == 0){

		UpdateData(results,threadData);
	}

	/*int r;
	for(r = 0; r < data->end - data->start; r++){

		printf("Result: %f Thread no: %d\n",*(results + (sizeof(double)*r)),data->threadNo);
	}*/
}

void printArray(){

	int x;
	int y;
	for(x =0; x < 5; x++){
		for(y = 0; y < 5; y++){

			printf("%f ",data[y][x]);
		}
		printf("\n");
	}
}


int main(int argc, char **argv) {

  pthread_t threads[NUM_THREADS];

  pthread_barrier_init(&calculateBarr,NULL,NUM_THREADS);
  pthread_barrier_init(&updateBarr,NULL,NUM_THREADS);

  int numPerThread = ((SIZE-2) * SIZE) / NUM_THREADS;
  int t;
  for(t = 0; t < NUM_THREADS; t++){

	  ThreadData* data = (ThreadData*)malloc(sizeof(ThreadData));
	  data->start = (t * numPerThread) + SIZE;
	  data->end = ((t+1)*numPerThread) + SIZE;
	  data->threadNo = t;
	  pthread_create(&threads[t],NULL,splitWork,data);
  }

  for(t = 0; t < NUM_THREADS; t++){

	  pthread_join(threads[t],NULL);
  }

  return EXIT_SUCCESS;
}
