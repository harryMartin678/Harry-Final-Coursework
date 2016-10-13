/*
 * frames.c
 *
 *  Created on: 12 Oct 2016
 *      Author: harry
 */

#include <stdlib.h>
#include <stdio.h>
#include "nodes.h"

struct Frame{

	struct SymbolNode* listHead;
	struct Frame* next;
	struct Frame* last;
};

struct Frame* currentFrame;

union Value{

	int intValue;
	NODE* functionBody;
};

struct SymbolNode{

	char* symbol;
	union Value value;
	struct SymbolNode* next;
};



void pushStack(){

	printf("push stack: %d\n",currentFrame == NULL);
	if(currentFrame == NULL){

		currentFrame = (struct Frame*)malloc(sizeof(struct Frame*));
		currentFrame->listHead = NULL;
		//printf("push stack list head: %d \n",currentFrame->listHead == NULL);

	}else{

		struct Frame* nextFrame = (struct Frame*)malloc(sizeof(struct Frame));
		nextFrame->last = currentFrame;
		nextFrame->listHead = NULL;
		currentFrame->next = nextFrame;

		currentFrame = nextFrame;
	}
}

void popStack(){

	struct Frame* lastFrame = currentFrame;
	currentFrame = currentFrame->last;
	free(lastFrame);
}

void addSymbol(char* symbol,union Value value){

	printf("ENTER addSymbol: %d \n",currentFrame->listHead == NULL);
	if(currentFrame->listHead == NULL){

		currentFrame->listHead = (struct SymbolNode*)malloc(sizeof(struct SymbolNode));
		currentFrame->listHead->symbol = symbol;
		currentFrame->listHead->value = value;
		currentFrame->listHead->next = NULL;

	}else{

		struct SymbolNode* tranverse = currentFrame->listHead;

		while(tranverse->next != NULL && tranverse->symbol != symbol){

			tranverse = tranverse->next;

		}

		if(tranverse->symbol){

			tranverse->value = value;

		}else{

			tranverse->next = (struct SymbolNode*)malloc(sizeof(struct SymbolNode));
			tranverse->next->symbol = symbol;
			tranverse->next->value = value;
		}
	}

}

union Value getValue(char* symbol){

	struct SymbolNode* tranverse = currentFrame->listHead;

	while(tranverse->next != NULL && tranverse->symbol != symbol){

		tranverse = tranverse->next;
	}

	//printf("%d\n",tranverse == NULL);
	return tranverse->value;
}
