/*
 * TestMPI.c
 *
 *  Created on: 1 Dec 2016
 *      Author: harry
 */
/*
 * MPIMain.c
 *
 *  Created on: 1 Dec 2016
 *      Author: harry
 */
#include <stdio.h>
#include <mpi.h>

int main(int argc,char** argv){

	int rc = MPI_Init(&argc,&argv);

	if(rc != MPI_SUCCESS){

		MPI_Abort(MPI_COMM_WORLD,rc);
	}

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	int size;
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	int n[3] = {1*rank,2*rank,3*rank};
	MPI_Send(n, 3, MPI_INT, (rank+1)%size, 99, MPI_COMM_WORLD);

	int m[3];
	MPI_Status stats;
	MPI_Recv(m,3,MPI_INT,(rank-1)%size,99,MPI_COMM_WORLD,&stats);

	int i;
	for(i = 0; i < 3; i++){

		printf("Data: %d To: %d From: %d \n",n[i],rank,m[0]);
	}
	printf("\n");

	MPI_Finalize();

	return 0;
}



