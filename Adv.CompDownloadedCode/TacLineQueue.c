/*
 * TacLineQueue.c
 *
 *  Created on: 19 Oct 2016
 *      Author: harry
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

struct TacLine{

	int variable;
	int operand1;
	int isVar1;
	int operand2;
	int isVar2;
	char operator;
	struct TacLine* next;
};

struct TacLine* head = NULL;
int size = 0;

void addToQueue(struct TacLine* next){

	if(head == NULL){

		head = next;
		size++;

	}else{

		struct TacLine* append = head;
		while(append->next != NULL){

			append = append->next;
		}

		append->next = next;
		size++;

	}
}

struct TacLine* getElement(int index){

	struct TacLine* next = head;
	while(index-- > 0){

		next = next ->next;
	}

	return next;
}


int getSize(){

	return size;
}





