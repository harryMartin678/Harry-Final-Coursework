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


//prints a frame used for debugging
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

//creates a new frame with this frame being connected to the last one
//frame* evn is the parent function frame if this is a child function
//if this is a normal function then env will be null
void pushStack(Frame* env,char* functionName){


	if(currentFrame == NULL){

		currentFrame = (Frame*)malloc(sizeof(Frame));
		currentFrame->listHead = NULL;
		currentFrame->last = NULL;
		currentFrame->no = 1;
		currentFrame->next = NULL;
		currentFrame->functionName = functionName;
		currentFrame->closure = NULL;
		globalFrame = currentFrame;

	}else{

		Frame* nextFrame = (Frame*)malloc(sizeof(Frame));
		nextFrame->last = currentFrame;
		nextFrame->listHead = NULL;
		nextFrame->no = currentFrame->no + 1;
		nextFrame->closure = env;
		currentFrame->next = nextFrame;
		currentFrame->functionName = functionName;
		currentFrame = nextFrame;
	}

}

//goes back to the last frame
void popStack(){
	Frame* lastFrame = currentFrame;
	currentFrame = currentFrame->last;

}

//gets the current environment
Frame* getEnvironment(){

	return currentFrame;
}

//adds a symbols to the current frame using pointer comparision
void addSymbol(char* symbol, Value value,int isVariableCreation){

	addSymbol0(symbol,value, 1,isVariableCreation);
}

//adds a symbols to the current frame using equality of strings
void addSymbolByEquality(char* symbol, Value value,int isVariableCreation){

	addSymbol0(symbol,value,0,isVariableCreation);
}

//if the current frame contains a symbol
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

//adds a symbol to the current mapping it to the value in value
void addSymbol0(char* symbol,Value value,int comparePointer,int isVariableCreation){

	//if the symbol already exists
	struct SymbolNode* update = getValue0(symbol,comparePointer);

	//if the symbol doesn't already exist or this symbol is to be added on to this scope anyway
	if(update == NULL || isVariableCreation){

		//add the symbol
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

		//update the value with this symbol
		update->value = value;
	}

}


//if a symbol is on the global frame and is a function
int isOnGlobalFrame(char* symbol){

	struct SymbolNode* next = globalFrame->listHead;

	while(next != NULL){

		if(strcmp(next->symbol,symbol) == 0 && next->value.isFunction == 1){

			return 1;
		}

		next = next->next;
	}

	return 0;
}


//get a value from the current frame or from a parent's frame(recursively)
struct SymbolNode* getValue0(char* symbol,int comparePointer){

	Frame* frame = currentFrame;
	struct SymbolNode* finalResult = NULL;

	//try this frame
	getValueFromFrame(frame,symbol,comparePointer,&finalResult);

	Frame* thisClosureEnv = frame->closure;
	int closureNo = 0;

	//if not this frame then try the parent's frame, then it's parent's frame etc...
	while(finalResult == NULL && thisClosureEnv != NULL){

		getValueFromFrame(thisClosureEnv,symbol,comparePointer,&finalResult);
		thisClosureEnv = thisClosureEnv->closure;
		closureNo++;
	}

	//check the global frame
	if(finalResult == NULL){

		getValueFromFrame(globalFrame,symbol,comparePointer,&finalResult);
	}

	//keep track of which closure this symbol was in
	if(finalResult != NULL){
		finalResult->closureNo = closureNo;
	}
	return finalResult;
}

//trys to find a value in a frame
void getValueFromFrame(Frame* frame,char* symbol,int comparePointer,
		struct SymbolNode** finalResult){

	struct SymbolNode* tranverse = frame->listHead;
	if(tranverse != NULL){

		//either compare by pointer or by equality
		//stop if there are no more mappings to search or the correct mapping
		//has been found
		while((tranverse->next != NULL && ((tranverse->symbol != symbol)
				|| (!comparePointer && strcmp(tranverse->symbol,symbol) != 0)))){

			tranverse = tranverse->next;
		}

		//if the correct mapping was found
		if((comparePointer && tranverse->symbol == symbol)
				|| (!comparePointer && strcmp(tranverse->symbol,symbol) == 0)){

			*finalResult = tranverse;
		}
	}
}

//get the last value added to the current frame
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

//changes all of the values in the current frame by a given amount
void changeAllInFrame(int amount){

	struct SymbolNode* node = currentFrame->listHead;

	while(node != NULL){

		node->value.valueType.intValue += amount;
		node = node->next;
	}
}

//get a value by equality of the symbol
Value getValueByEquality(char* symbol,int* closureNo){

	struct SymbolNode* sym = getValue0(symbol,0);
	*closureNo = sym->closureNo;
	return sym->value;
}

//get a value by pointer comparision of the symbol
Value getValue(char* symbol){

	return getValue0(symbol,1)->value;
}

