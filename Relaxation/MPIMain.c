/*
 * MPIMain.c
 *
 *  Created on: 1 Dec 2016
 *      Author: harry
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
//#include <math.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

struct Allocation{

	int rank;
	int start;
	int end;
};

typedef struct Allocation Allocation;
double dabs(double number);

void createData(double*** data,Allocation alloc,int arraySize,int noOfProcessors,int xSize,int ySize);
void freeArray(double*** array,int xSize,int ySize);
int findArrayPosProcess(int row,int noOfRows,int noOfProcesses);
Allocation findSegment(int processNo,int noOfProcesses,int noOfRows);
double average(int x, int y,double** data);
void printData(double** data,int xSize,int ySize);
void sendMessage(double** data,int rank,int editCol,Allocation info,int ySize);
void recieveMessage(double** data,int rank,int editCol,int ySize);
int calPercision(double one, double two);
double dabs(double number);
void doRelaxation(double** data,Allocation alloc,int arraySize,int noOfProcessors,
		int xSize,int ySize,int mode);
void freeArray(double*** array,int xSize,int ySize);
double assignValue(int x, int y,int arraySize);
void createData(double*** data,Allocation alloc,int arraySize,int noOfProcessors,
		int xSize,int ySize);


int findArrayPosProcess(int row,int noOfRows,int noOfProcesses){

	int index = row;
	int remainder = noOfRows%noOfProcesses;
	int smallDivision = noOfRows/noOfProcesses;

	int gap = (noOfProcesses - remainder) * smallDivision;

	if(index < gap){

		return index / smallDivision;

	}else{

		return ((index - gap) / (smallDivision+1)) + gap/smallDivision;
	}

}

Allocation findSegment(int processNo,int noOfProcesses,int noOfRows){

	int remainder = noOfRows%noOfProcesses;
	int smallDivision = noOfRows/noOfProcesses;

	int gap = (noOfProcesses - remainder) * smallDivision;

	int index = processNo * smallDivision;

	Allocation divide;
	if(index < gap){

		divide.start = index;
		divide.end = index + smallDivision-1;

	}else{

		int newIndex = gap + ((processNo - (gap/smallDivision)) * (smallDivision+1));
		divide.start = newIndex;
		divide.end = newIndex + smallDivision;
	}

	return divide;
}

//calculates the average of four neighbours. The two ints x and y are the position in the array
//that is to be averaged.
double average(int x, int y,double** data){

	return (data[x-1][y] + data[x+1][y] + data[x][y+1] + data[x][y-1])/4.0;
}

void printData(double** data,int xSize,int ySize){

	int x;
	int y;
	for(x = 0; x < xSize; x++){
		for(y = 0; y < ySize; y++){

			printf("%f ",data[x][y]);
		}
		printf("\n");
	}
}

void sendMessage(double** data,int rank,int editCol,Allocation info,int ySize){

	MPI_Request request;
	MPI_Isend(data[editCol],ySize,MPI_DOUBLE,rank,99,MPI_COMM_WORLD,&request);
}

void recieveMessage(double** data,int rank,int editCol,int ySize){

	MPI_Status stat;
	MPI_Recv(data[editCol],ySize,MPI_DOUBLE,rank,99,MPI_COMM_WORLD,&stat);

}

int calPercision(double one, double two){

	return dabs(one - two) < 0.25;
}

double dabs(double number){

	return (number < 0 ? -number : number);
}

void doRelaxation(double** data,Allocation alloc,int arraySize,int noOfProcessors,
		int xSize,int ySize,int mode){

	double** copy = (double**)malloc(sizeof(double*)*xSize);
	createData(&copy,alloc,arraySize,noOfProcessors,xSize,ySize);
	double*** write = &copy;
	double*** read = &data;
	int endLoop = 0;
	int* terminate = (int*)malloc(sizeof(int));

	while(!endLoop){

		int y;
		int x;

		*terminate = 1;


		for(y = 1; y < ySize - 1; y++){
			for(x = 1; x < xSize-1; x++){

				(*write)[x][y] = average(x,y,(*read));

				if(*terminate && !calPercision((*write)[x][y],(*read)[x][y])){

					*terminate = 0;
				}
			}
		}

		if(alloc.rank != noOfProcessors-1){

			sendMessage((*write),alloc.rank+1,xSize-2,alloc,ySize);

		}

		if(alloc.rank != 0){

			sendMessage((*write),alloc.rank-1,1,alloc,ySize);
		}

		if(alloc.rank != 0){

			recieveMessage((*write),alloc.rank-1,0,ySize);
		}

		if(alloc.rank != noOfProcessors-1){

			recieveMessage((*write),alloc.rank+1,xSize-1,ySize);
		}

		if(alloc.rank == 0){

			endLoop = 1;
			MPI_Status stat;
			int s;
			for(s = 1; s < noOfProcessors; s++){

				int hasEnded;
				MPI_Recv(&hasEnded,1,MPI_INT,s,98,MPI_COMM_WORLD,&stat);
				endLoop = endLoop && hasEnded;
			}

			endLoop = *terminate && endLoop;


		}else{

			MPI_Send(terminate,1,MPI_INT,0,98,MPI_COMM_WORLD);

		}

		MPI_Bcast(&endLoop,1,MPI_INT,0,MPI_COMM_WORLD);

		double*** temp = read;
		read = write;
		write = temp;
	}

	if(mode == 1){

		if(alloc.rank == 0){


			printf("Rank:0\n");
			printData((*read),xSize,ySize);
			int r;
			for(r = 1; r < noOfProcessors; r++){


				MPI_Status stat;
				int msgSize;
				MPI_Recv(&msgSize,sizeof(int),MPI_INT,r,96,MPI_COMM_WORLD,&stat);


				printf("Rank:%d\n",r);
				int s;
				double* msg = (double*)malloc(sizeof(double)*ySize);
				for(s = 0; s < msgSize; s++){

					MPI_Recv(msg,ySize,MPI_DOUBLE,r,101 + s,MPI_COMM_WORLD,&stat);
					int m;
					for(m = 0; m < ySize; m++){

						printf("%f ",msg[m]);
					}
					printf("\n");
				}


			}

			printf("PROCESSORS:%d\nSIZE:%d\nEND\n",noOfProcessors,ySize);

		}else{
			MPI_Request request;
			MPI_Isend(&xSize,sizeof(int),MPI_INT,0,96,MPI_COMM_WORLD,&request);
			int t;
			for(t = 0; t < xSize; t++){
				MPI_Send((*read)[t],ySize,MPI_DOUBLE,0,101 + t,MPI_COMM_WORLD);
			}


		}
	}

	freeArray(read,xSize,ySize);
	freeArray(write,xSize,ySize);
	free(terminate);


}

void freeArray(double*** array,int xSize,int ySize){

	int x;
	for(x = 0; x < xSize; x++){

		free((*array)[x]);
	}

	free((*array));
}


double assignValue(int x, int y,int arraySize){

	//the boundaries are all 1.0s
	if(x == 0 || y == 0 || x == arraySize-1 || y == arraySize-1){

		return 1.0;

	}else{

		//the inside of the array is 1.0 or 0.0s
		if((x + y) % 4 == 0 || (x + y) % 7 == 0){

			return 1.0;
		}else{
			return 0.0;
		}

	}
}

//creates an array of size SIZE
//creates the same array each time
void createData(double*** data,Allocation alloc,int arraySize,int noOfProcessors,
		int xSize,int ySize){

	int x;
	int y;

	for(x = alloc.start-(alloc.rank != 0); x <= alloc.end + (alloc.rank != noOfProcessors-1); x++){
		(*data)[x - (alloc.start-(alloc.rank != 0))] = (double*)malloc(sizeof(double)*ySize);
		for(y = 0; y < ySize; y++){

			(*data)[x- (alloc.start-(alloc.rank != 0))][y] = assignValue(x,y,arraySize);

		}
	}

}


int main(int argc,char** argv){

	if(argc < 3){

		printf("Too few arguments: sizeOfGrid,mode\n");
		exit(0);
	}

	int arraySize = atoi(argv[1]);
	int mode = atoi(argv[2]);

	if(mode != 1 && mode != 2){

		printf("Invalid mode %d\n",mode);
		exit(0);
	}

	int rc = MPI_Init(&argc,&argv);

	struct timeval start, end;
	if(mode == 2){
		//get the time
		gettimeofday(&start, NULL);
	}


	if(rc != MPI_SUCCESS){

		MPI_Abort(MPI_COMM_WORLD,rc);
	}

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	int noOfProcessors;
	MPI_Comm_size(MPI_COMM_WORLD,&noOfProcessors);

	Allocation alloc = findSegment(rank,noOfProcessors,arraySize);
	alloc.rank = rank;

	int ySize = arraySize;
	int xSize = (alloc.end-alloc.start)+ (alloc.rank != noOfProcessors-1)+(alloc.rank != 0)+1;
	double** data = (double**)malloc(sizeof(double*)*xSize);
	createData(&data,alloc,arraySize,noOfProcessors,xSize,ySize);
	doRelaxation(data,alloc,arraySize,noOfProcessors,xSize,ySize,mode);

	MPI_Finalize();

	if(mode == 2){
		gettimeofday(&end, NULL);
		printf("No of Processors:%d Size:%d Rank:%d Time Taken(seconds):%f\n",
				noOfProcessors,arraySize,rank,((end.tv_sec * 1000000 + end.tv_usec)-
						(start.tv_sec * 1000000 + start.tv_usec))/1000000.0);
	}

	return 0;
}

