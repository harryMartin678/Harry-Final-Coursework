/*
 * hash.c
 *
 *  Created on: 9 Oct 2016
 *      Author: harry
 */
#include <stdlib.h>
#include <stdio.h>
//https://www.tutorialspoint.com/data_structures_algorithms/hash_table_program_in_c.htm
#define SIZE 20
/*#ifndef NULL
#define NULL   ((void *) 0)
#endif*/

struct HashElement{

	int value;
	char* variable;
	struct HashElement* next;

};


struct HashElement* hashTable[SIZE];

int hashCode(char* variable){

	int h = 0;
	while (*variable != '\0') {
	  h = (h<<4) ^ *variable++;
	}
	return (0x7fffffff&h) % SIZE;
}

void Insert(char* variable,int value){

	int hashIndex = 0;

	if((hashIndex = Contains(variable)) != -1){

		hashTable[hashIndex]->value = value;

	}else{

		printf("INSERT \n");
		Insert0(variable,value);
	}
}

void Insert0(char* variable,int value){

	int hashIndex = hashCode(variable);

	struct HashElement* element = (struct HashElement*)malloc(sizeof(struct HashElement));
	element->variable = variable;
	element->value = value;

	struct HashElement* next = hashTable[hashIndex];

	//printf("Insert null: %d\n",next == NULL);
	while(next != NULL){

		next = next->next;
	}

	//printf("Insert: %c %d %d \n",element->variable,element->value,hashIndex);
	next = element;


}

void printTable(){

	int i;

	printf("START PRINT TABLE\n");
	for(i = 0; i < SIZE; i++){

		if(hashTable[i] == NULL){

			continue;
		}

		printf("%c %d\n",hashTable[i]->variable,hashTable[i]->value);
	}
	printf("END PRINT TABLE\n");
}

int Contains(char* variable){

	int hashIndex = hashCode(variable);

	struct HashElement* next = hashTable[hashIndex];

	while(next != NULL && next->variable != variable){

		next = next->next;
	}

	if(next == NULL){

		return -1;
	}

	return hashIndex;

}


int Search(char* variable,int* error){

	int hashIndex = hashCode(variable);

	struct HashElement* next = hashTable[hashIndex];

	while(next != NULL && next->variable != variable){

		next = next->next;
	}

	if(next == NULL){

		*error = 1;
		return -1;
	}

	*error = 0;
	return next->value;

}

