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

	char* variable;
	int paramType;
	int isVar1Temp;
	char* variable2;
	int isVar2Temp;
	int operand1;
	int isVar1;
	int operand2;
	int isVar2;
	int operator;
	int isSimple;
	int isStatement;
	int isVariableEq;
	int isNext;
	int isRegisterFunctionCall;
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
		while(append->isNext == 2){

			append = append->next;
		}

		append->next = next;
		append->isNext = 2;
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





