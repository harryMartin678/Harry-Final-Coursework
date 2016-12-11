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
	char* parentFunctionName;
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
void addSymbol0(char* symbol,Value value,int comparePointer,int isVariableCreation);
void printFrame(Frame* frame);
void pushStack(Frame* env,char* functionName);
void popStack();
Frame* getEnvironment();
void addSymbol(char* symbol, Value value,int isVariableCreation);
void addSymbolByEquality(char* symbol, Value value,int isVariableCreation);
int containsSymbol(char* symbol);
int isOnGlobalFrame(char* symbol);
struct SymbolNode* getValue0(char* symbol,int comparePointer);
void getValueFromFrame(Frame* frame,char* symbol,int comparePointer,
		struct SymbolNode** finalResult);
Value getLastValue();
void changeAllInFrame(int amount);
Value getValueByEquality(char* symbol,int* closureNo);
Value getValue(char* symbol);


void printFrame(Frame* frame){

	if(frame == NULL){

		printf("frame is null\n");

	}else{

		printf("Print Bindings begin \n");
		struct SymbolNode* tranverse = frame->listHead;
		while(tranverse != NULL){

			printf("\tBinding %s ",tranverse->symbol);
			printf("%d:\n",tranverse->value.valueType.intValue);
			tranverse = tranverse->next;
		}
		printf("Print Bindings end\n");
	}
}

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
		//printf("malloc: %d\n",sizeof(Frame));
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
		//printf("malloc: %d\n",sizeof(Frame));
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

void addSymbol(char* symbol, Value value,int isVariableCreation){

	addSymbol0(symbol,value, 1,isVariableCreation);
}

void addSymbolByEquality(char* symbol, Value value,int isVariableCreation){

	addSymbol0(symbol,value,0,isVariableCreation);
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


void addSymbol0(char* symbol,Value value,int comparePointer,int isVariableCreation){

//	printf("symbol %s\n",symbol);
	struct SymbolNode* update = getValue0(symbol,comparePointer);

	if(update == NULL || isVariableCreation){

		if(currentFrame->listHead == NULL){

			currentFrame->listHead = (struct SymbolNode*)malloc(sizeof(struct SymbolNode));
			currentFrame->listHead->symbol = symbol;
			currentFrame->listHead->value = value;
			currentFrame->listHead->next = NULL;

		}else{

			struct SymbolNode* tranverse = currentFrame->listHead;
			while(tranverse->next != NULL){

				tranverse = tranverse->next;

			}

			tranverse->next = (struct SymbolNode*)malloc(sizeof(struct SymbolNode));
			tranverse->next->symbol = symbol;
			tranverse->next->value = value;
			tranverse->next->next = NULL;

		}
	}else{

		update->value = value;
	}

}



int isOnGlobalFrame(char* symbol){

	struct SymbolNode* next = globalFrame->listHead;

	while(next != NULL){

		if(strcmp(next->symbol,symbol) == 0){

			return 1;
		}

		next = next->next;
	}

	return 0;
}



struct SymbolNode* getValue0(char* symbol,int comparePointer){

	Frame* frame = currentFrame;
	struct SymbolNode* finalResult = NULL;

	//printFrame(frame);

	getValueFromFrame(frame,symbol,comparePointer,&finalResult);
	//printf("1: %d\n",finalResult == NULL);

	Frame* thisClosureEnv = frame->closure;
	int closureNo = 0;

	while(finalResult == NULL && thisClosureEnv != NULL){

		//printf("2: %s %d %d\n",symbol,finalResult == NULL,thisClosureEnv != NULL);
		//printFrame(thisClosureEnv);
		getValueFromFrame(thisClosureEnv,symbol,comparePointer,&finalResult);
		thisClosureEnv = thisClosureEnv->closure;
		closureNo++;
	}
	//printf("2: %d %d\n",finalResult == NULL,thisClosureEnv != NULL);

	if(finalResult == NULL){

		getValueFromFrame(globalFrame,symbol,comparePointer,&finalResult);
	}
	//printf("3: %d\n",finalResult == NULL);

	//if(finalResult == NULL){
	//	printf("symbol: %s %d\n",symbol,thisClosureEnv == NULL);
	//}

	//printFrame(thisClosureEnv);
	if(finalResult != NULL){
		finalResult->closureNo = closureNo;
	}else{
		//printf("couldn't find %s\n",symbol);
	}
	return finalResult;
}

void getValueFromFrame(Frame* frame,char* symbol,int comparePointer,
		struct SymbolNode** finalResult){

	struct SymbolNode* tranverse = frame->listHead;
	if(tranverse != NULL){

		while((tranverse->next != NULL && ((tranverse->symbol != symbol)
				|| (!comparePointer && strcmp(tranverse->symbol,symbol) != 0)))){

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
		value.valueType.intValue = -4;
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

	struct SymbolNode* sym = getValue0(symbol,0);
	*closureNo = sym->closureNo;
	return sym->value;
}

Value getValue(char* symbol){

	return getValue0(symbol,1)->value;
}

/*Value backTrackValue(char* symbol,int backTrack){

	return getValue0(symbol,backTrack,1)->value;
}*/
