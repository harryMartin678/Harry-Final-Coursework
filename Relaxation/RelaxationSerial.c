/*
 * RelaxationSerial.c
 *
 *  Created on: 15 Nov 2016
 *      Author: harry
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int findArrayPosProcess(int x, int y,int arraySize,int noOfProcesses){

	return ((x / arraySize) + (y % arraySize)) / min(noOfProcesses,arraySize*arraySize);
}

int main(int argc, char **argv){

	int arraySize = 10;
	int noOfProcesses = 3;

	for(int x = 0; x < arraySize; x++){
		for(int y = 0; y < arraySize; y++){

			printf("%d ",findArrayPosProcess(x,y,arraySize,noOfProcesses));
		}
	}


}
