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
	int no;
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
		currentFrame->last = NULL;
		currentFrame->no = 1;
		//printf("push stack list head: %d \n",currentFrame->listHead == NULL);

	}else{

		//printCurrentFrame();
		struct Frame* nextFrame = (struct Frame*)malloc(sizeof(struct Frame));
		nextFrame->last = currentFrame;
		nextFrame->listHead = NULL;
		nextFrame->no = currentFrame->no + 1;
		currentFrame->next = nextFrame;

		currentFrame = nextFrame;
	}
	//printCurrentFrame();
}

void popStack(){

	//printCurrentFrame();
	struct Frame* lastFrame = currentFrame;
	currentFrame = currentFrame->last;
	free(lastFrame);

}

void addSymbol(char* symbol,union Value value){

	printf("ENTER addSymbol: %s %d %d \n",symbol,value.intValue,currentFrame->no);
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

		if(tranverse->symbol == symbol){

			tranverse->value = value;

		}else{

			tranverse->next = (struct SymbolNode*)malloc(sizeof(struct SymbolNode));
			tranverse->next->symbol = symbol;
			tranverse->next->value = value;
		}
	}

}

void printCurrentFrame(){

	if(currentFrame != NULL){
		printf("Print Bindings begin\n");
		struct SymbolNode* tranverse = currentFrame->listHead;
		while(tranverse != NULL){

			printf("Binding %s %d: \n",tranverse->symbol,tranverse->value.intValue);
			tranverse = tranverse->next;
		}
		printf("Print Bindings end\n");
	}
}



union Value getValue0(char* symbol,int backTrack){

	printf("get value: %s %d\n",symbol,backTrack);
	struct Frame* frame = currentFrame;
	struct SymbolNode* finalResult;

	while(backTrack-- > 0){

		frame = frame->last;
	}

	while(frame != NULL){

		struct SymbolNode* tranverse = frame->listHead;

		while(tranverse->next != NULL && tranverse->symbol != symbol){

			tranverse = tranverse->next;
		}

		if(tranverse->symbol == symbol){

			finalResult = tranverse;
			break;

		}else{

			frame = currentFrame->last;
		}

	}

	return finalResult->value;
}

union Value getValue(char* symbol){

	return getValue0(symbol,0);
}

union Value backTrackValue(char* symbol){

	return getValue0(symbol,1);
}
