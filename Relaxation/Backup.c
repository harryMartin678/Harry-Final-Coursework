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

/*Allocation findSegment(int processNo,int noOfProcesses,int arraySize){

	int noOfElements = arraySize*arraySize;
	int remainder = noOfElements%noOfProcesses;
	int smallDivision = noOfElements/noOfProcesses;

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
}*/

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

void sendMessage(double** data,int rank,int editCol,Allocation info,int xSize,double* slice){

	//double* slice = (double*)malloc(sizeof(double)*xSize-2);
	int x;
	//printf("sent to %d %d, this rank %d %d\n",rank,editCol,info.rank,xSize);
	for(x = 1; x < xSize - 1; x++){

		//slice[x] = data[x][editCol];
		slice[x] = 1;
		//printf("%f ",data[x][editCol]);
	}
	//slice[xSize-1] = (int)*terminate;
	//printf("\n");
	//printf("x: %d\n",x);
	/*double* slice = (double*)malloc(sizeof(double)*ySize);
	int s;
	for(s = 1; s < ySize-1; s++){

		slice[s-1] = (*data)[editCol][s];
		printf("%d\n",slice[s-1]);
	}*/

	/*int s;
	for(s = 0; s < ySize; s++){

		printf("%f ",data[editCol][s]);
	}
	printf("\n");*/


	MPI_Request request;
	MPI_Isend(slice,xSize,MPI_DOUBLE,rank,99,MPI_COMM_WORLD,&request);
}

void recieveMessage(double** data,int rank,int editCol,int xSize,double* slice){

	MPI_Status stat;
	//double* slice = (double*)malloc(sizeof(double)*ySize);
	/*int s;
	for(s = 0; s < ySize; s++){

		printf("%f ",data[editCol][s]);
	}
	printf("\nRank: %d\n",rank);*/
	MPI_Recv(slice,xSize,MPI_DOUBLE,rank,99,MPI_COMM_WORLD,&stat);

	/*int s;
	for(s = 0; s < ySize-1; s++){

		(*data)[editCol][s] = slice[s-1];
	}*/
	//printf("recieved from %d, this rank %d\n",rank,currentRank);
	int x;
	for(x = 1; x < xSize-1; x++){

		data[x][editCol] = slice[x];
		//printf("%f ",data[x][editCol]);
	}
	//*terminate = (int)slice[xSize-1];
	//printf("\n");
}

int calPercision(double one, double two){

	return dabs(one - two) < 0.1;
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
	int* upRankEnded = (int*)malloc(sizeof(int));
	*upRankEnded = 0;
	int* downRankEnded = (int*)malloc(sizeof(int));
	*downRankEnded = 0;

	double* slice = (double*)malloc(sizeof(double)*xSize);
	//struct timeval start,one,two,three,four,five;


	while(!endLoop){

		int y;
		int x;

		*terminate = 1;

		//gettimeofday(&start, NULL);
		for(y = 1; y < ySize - 1; y++){
			for(x = 1; x < xSize-1; x++){

				(*write)[x][y] = average(x,y,(*read));

				if(*terminate && !calPercision((*write)[x][y],(*read)[x][y])){

					*terminate = 0;
				}
			}
		}
		//gettimeofday(&one, NULL);

		if(alloc.rank != noOfProcessors-1){

			sendMessage((*write),alloc.rank+1,ySize-2,alloc,xSize,slice);

		}

		if(alloc.rank != 0){

			sendMessage((*write),alloc.rank-1,1,alloc,xSize,slice);
		}

		if(alloc.rank != noOfProcessors-1){

			MPI_Send(terminate,sizeof(int),MPI_INT,alloc.rank+1,98,MPI_COMM_WORLD);
		}

		if(alloc.rank != 0){

			MPI_Send(terminate,sizeof(int),MPI_INT,alloc.rank-1,98,MPI_COMM_WORLD);
		}

		//gettimeofday(&two, NULL);
		if(alloc.rank != 0 && !*downRankEnded){

			recieveMessage((*write),alloc.rank-1,0,xSize,slice);
		}

		//gettimeofday(&three, NULL);
		if(alloc.rank != noOfProcessors-1 && !*upRankEnded){

			recieveMessage((*write),alloc.rank+1,ySize-1,xSize,slice);
		}

		//gettimeofday(&four, NULL);
		if(alloc.rank != 0 && !*downRankEnded){

			MPI_Status stat;
			MPI_Recv(downRankEnded,sizeof(int),MPI_INT,alloc.rank-1,98,MPI_COMM_WORLD,&stat);
		}
		if(alloc.rank != noOfProcessors-1 && !*upRankEnded){

			MPI_Status stat;
			MPI_Recv(upRankEnded,sizeof(int),MPI_INT,alloc.rank+1,98,MPI_COMM_WORLD,&stat);
		}
		//gettimeofday(&five, NULL);

		endLoop = (alloc.rank == 0 || *downRankEnded) && (alloc.rank == noOfProcessors-1 || *upRankEnded)
				&& *terminate;


		//printf("%f %f %f %f %f\n",(((one.tv_sec * 1000000 + one.tv_usec)-
			//	(start.tv_sec * 1000000 + start.tv_usec))/1000000.0),(((two.tv_sec * 1000000 + two.tv_usec)-
						//(one.tv_sec * 1000000 + one.tv_usec))/1000000.0),(((three.tv_sec * 1000000 + three.tv_usec)-
								//(two.tv_sec * 1000000 + two.tv_usec))/1000000.0),
								//(((four.tv_sec * 1000000 + four.tv_usec)-
										//(three.tv_sec * 1000000 + three.tv_usec))/1000000.0),
												//(((five.tv_sec * 1000000 + five.tv_usec)-
												//(four.tv_sec * 1000000 + four.tv_usec))/1000000.0));
		double*** temp = read;
		read = write;
		write = temp;
	}

	if(mode == 1){
		//printf("Rank:%d\n",alloc.rank);
		//printData(*read,xSize,ySize);
		//printf("End Rank:%d\n",alloc.rank);
		if(alloc.rank == 0){

			//double*** total = (double***)malloc(sizeof(double**)*noOfProcessors);

			//total[0] = *read;

			printf("Rank:0\n");
			printData((*read),xSize,ySize);
			int r;
			for(r = 1; r < noOfProcessors; r++){


				MPI_Status stat;
				int msgSize;
				MPI_Recv(&msgSize,sizeof(int),MPI_INT,r,96,MPI_COMM_WORLD,&stat);


				printf("Rank:%d\n",r);
				int s;
				double* msg = (double*)malloc(sizeof(double)*xSize);
				for(s = 0; s < xSize; s++){

					MPI_Recv(msg,msgSize,MPI_DOUBLE,r,101 + s,MPI_COMM_WORLD,&stat);
					int m;
					for(m = 0; m < msgSize; m++){

						printf("%f ",msg[m]);
					}
					printf("\n");
				}


			}

			printf("PROCESSORS:%d\nSIZE:%d\nEND\n",noOfProcessors,xSize);

		}else{
			MPI_Request request;
			MPI_Isend(&ySize,sizeof(int),MPI_INT,0,96,MPI_COMM_WORLD,&request);
			//printf("rank: %d send: %d\n",alloc.rank,ySize);
			int t;
			for(t = 0; t < xSize; t++){
				MPI_Send((*read)[t],ySize,MPI_DOUBLE,0,101 + t,MPI_COMM_WORLD);
			}


		}
	}

}

/*double* getColumn(double** matrix){

	double* col = (double*)malloc(sizeof(double));


}*/

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

	//for(x = alloc.start-(alloc.rank != 0); x < alloc.end + (alloc.rank != noOfProcessors-1); x++){
	for(x = 0; x < xSize; x++){
		(*data)[x] = (double*)malloc(sizeof(double)*ySize);
		for(y = alloc.start-(alloc.rank != 0); y < alloc.end + (alloc.rank != noOfProcessors-1); y++){

			(*data)[x][y- (alloc.start-(alloc.rank != 0))] = assignValue(x,y,arraySize);

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

	int xSize = arraySize;
	int ySize = (alloc.end-alloc.start)+ (alloc.rank != noOfProcessors-1)+(alloc.rank != 0)+1;
	double** data = (double**)malloc(sizeof(double*)*xSize);
	createData(&data,alloc,arraySize,noOfProcessors,xSize,ySize);
	doRelaxation(data,alloc,arraySize,noOfProcessors,xSize,ySize,mode);

	/*int n = 2;
	MPI_Status stat;
	if(alloc.rank == 0){
		MPI_Send(&n,1,MPI_INT,1,94,MPI_COMM_WORLD);
	}else if(alloc.rank == 1){

		MPI_Recv(&n,1,MPI_INT,0,94,MPI_COMM_WORLD,&stat);
	}

	printf("%d\n",n);*/

	MPI_Finalize();

	if(mode == 2){
		gettimeofday(&end, NULL);
		printf("No of Processors:%d Size:%d Rank:%d Time Taken(seconds):%f\n",
				noOfProcessors,arraySize,rank,((end.tv_sec * 1000000 + end.tv_usec)-
						(start.tv_sec * 1000000 + start.tv_usec))/1000000.0);
	}

	return 0;
}

