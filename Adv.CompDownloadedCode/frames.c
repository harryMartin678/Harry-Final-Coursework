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
	struct Frame* closure;
	char* functionName;
	int no;
};

typedef struct Frame Frame;

Frame* currentFrame;
Frame* globalFrame;
//Frame* closureEnv;


struct Closure{

	NODE* functionBody;
	Frame* env;
};

union ValueType{

	int intValue;
	struct Closure* closure;
};

struct Value{

	int isFunction;
	union ValueType valueType;
};

typedef struct Value Value;

struct SymbolNode{

	char* symbol;
	Value value;
	struct SymbolNode* next;
	int closureNo;
};

typedef struct Value Value;
typedef union ValueType ValueType;

void getValueFromFrame(Frame* frame,char* symbol,int comparePointer,
		struct SymbolNode** finalResult);
void addSymbol0(char* symbol,Value value,int comparePointer);


void pushStack(Frame* env,char* functionName){

	//Frame* oldClosure = closureEnv;
	//closureEnv = env;
	//if(closureEnv != NULL){

	//	closureEnv->functionName = functionName;
	//}
	//env->closure = closureEnv;
	//printf("\npush stack \n");

	if(currentFrame == NULL){

		currentFrame = (Frame*)malloc(sizeof(Frame));
		currentFrame->listHead = NULL;
		currentFrame->last = NULL;
		currentFrame->no = 1;
		currentFrame->next = NULL;
		currentFrame->functionName = functionName;
		currentFrame->closure = NULL;
		globalFrame = currentFrame;
		//printf("push stack list head: %d \n",currentFrame->listHead == NULL);

	}else{

		//printCurrentFrame();
		Frame* nextFrame = (Frame*)malloc(sizeof(Frame));
		nextFrame->last = currentFrame;
		nextFrame->listHead = NULL;
		nextFrame->no = currentFrame->no + 1;
		nextFrame->closure = env;
		currentFrame->next = nextFrame;
		currentFrame->functionName = functionName;
		currentFrame = nextFrame;
	}

	//printCurrentFrame();
}

void popStack(){
	//printf("\npop stack \n");
	//printCurrentFrame();
	Frame* lastFrame = currentFrame;
	currentFrame = currentFrame->last;
	//free(lastFrame);

}

//void addSymbol(char* symbol, Value value){

	//addSymbol0(symbol,value,0);
//}

Frame* getEnvironment(){

	return currentFrame;
}

void addSymbol(char* symbol, Value value){

	addSymbol0(symbol,value, 1);
}

void addSymbolByEquality(char* symbol, Value value){

	addSymbol0(symbol,value,0);
}

int containsSymbol(char* symbol){

	if(currentFrame == NULL || currentFrame->listHead == NULL){

		return 0;
	}

	struct SymbolNode* tranverse = currentFrame->listHead;

	while(tranverse != NULL){

		if(strcmp(tranverse->symbol,symbol) == 0){

			return 1;
		}

		tranverse = tranverse->next;
	}

	return 0;

}

void addSymbol0(char* symbol,Value value,int comparePointer){


	//printf("ENTER addSymbol: %s %d %d \n",symbol,value.intValue,currentFrame->no);
	if(currentFrame->listHead == NULL){

		currentFrame->listHead = (struct SymbolNode*)malloc(sizeof(struct SymbolNode));
		currentFrame->listHead->symbol = symbol;
		currentFrame->listHead->value = value;
		currentFrame->listHead->next = NULL;

	}else{

		struct SymbolNode* tranverse = currentFrame->listHead;

		while(tranverse->next != NULL && ((comparePointer && tranverse->symbol != symbol)
				|| (!comparePointer && strcmp(tranverse->symbol,symbol) == 0))){

			tranverse = tranverse->next;

		}

		if(tranverse->symbol == symbol){

			tranverse->value = value;

		}else{

			tranverse->next = (struct SymbolNode*)malloc(sizeof(struct SymbolNode));
			tranverse->next->symbol = symbol;
			tranverse->next->value = value;
			tranverse->next->next = NULL;
		}
	}

}

void printFrame(Frame* frame){

	if(frame != NULL){
		printf("Print Bindings begin %s\n",frame->functionName);
		struct SymbolNode* tranverse = frame->listHead;
		while(tranverse != NULL){

			printf("\tBinding %s %d: \n",tranverse->symbol,tranverse->value.valueType.intValue);
			tranverse = tranverse->next;
		}
		printf("Print Bindings end\n");
	}else{

		printf("frame is null\n");
	}
}



struct SymbolNode* getValue0(char* symbol,int backTrack,int comparePointer){

	Frame* frame = currentFrame;
	struct SymbolNode* finalResult = NULL;

	int initialBackTrack = backTrack;
	//Frame* thisClosureEnv = closureEnv;
	while(backTrack-- > 0){

		frame = frame->last;
		//if(thisClosureEnv != NULL){
			//thisClosureEnv = thisClosureEnv -> last;
		//}
	}

	getValueFromFrame(frame,symbol,comparePointer,&finalResult);

	Frame* thisClosureEnv = frame->closure;
	int closureNo = 0;
	if(finalResult == NULL && thisClosureEnv != NULL){

		while(finalResult == NULL && thisClosureEnv != NULL){
			//printf("1: %s is null %d\n",symbol,finalResult == NULL);
			getValueFromFrame(thisClosureEnv,symbol,comparePointer,&finalResult);
			thisClosureEnv = thisClosureEnv->closure;
			closureNo++;
		}
		///printf("2: %s is null %d\n",symbol,finalResult == NULL);
	}

	//printf("3: %s is null %d\n",symbol,finalResult == NULL);
	if(finalResult == NULL){

		getValueFromFrame(globalFrame,symbol,comparePointer,&finalResult);
	}

	//printf("4: %s is null %d\n",symbol,finalResult == NULL);
	//printf("enter get value %d\n",initialBackTrack);
	//if(thisClosureEnv != NULL){
	if(finalResult == NULL){
		printf("backtrack: %d symbol: %s %d\n",initialBackTrack,symbol,thisClosureEnv == NULL);
	}
		//printFrame(thisClosureEnv);
	//}
	//printf("final result = %d, symbol: %s\n",finalResult == NULL,symbol);
	finalResult->closureNo = closureNo;
	return finalResult;
}

void getValueFromFrame(Frame* frame,char* symbol,int comparePointer,
		struct SymbolNode** finalResult){

	struct SymbolNode* tranverse = frame->listHead;
	if(tranverse != NULL){

		while((tranverse->next != NULL && tranverse->symbol != symbol)){

			tranverse = tranverse->next;
		}

		if((comparePointer && tranverse->symbol == symbol)
				|| (!comparePointer && strcmp(tranverse->symbol,symbol) == 0)){

			*finalResult = tranverse;
		}
	}
}

Value getLastValue(){

	struct SymbolNode* node = currentFrame->listHead;

	if(node == NULL){

		Value value;
		value.valueType.intValue = 0;
		return value;
	}

	while(node->next != NULL){

		node = node->next;
	}

	return node->value;
}

void changeAllInFrame(int amount){

	struct SymbolNode* node = currentFrame->listHead;

	while(node != NULL){

		//printf("NEXT CHANGE FRAME BEFORE %d\n",node->value.intValue);
		node->value.valueType.intValue += amount;
		//printf("NEXT CHANGE FRAME AFTER %d\n",node->value.intValue);
		node = node->next;
	}
}

Value getValueByEquality(char* symbol,int* closureNo){

	struct SymbolNode* sym = getValue0(symbol,0,0);
	*closureNo = sym->closureNo;
	return sym->value;
}

Value getValue(char* symbol){

	return getValue0(symbol,0,1)->value;
}

Value backTrackValue(char* symbol,int backTrack){

	return getValue0(symbol,backTrack,1)->value;
}
