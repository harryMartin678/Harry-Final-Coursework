#include "frames.h"
#include "TacLineQueue.h"
#include <stdio.h>
#include <stdlib.h>


struct Param{

	int number;
	int memoryPos;
	int value;
	struct Param* next;
};

typedef struct Param Param;

Param* head;

void addParam(int value){

	Param* next = (Param*)malloc(sizeof(Param));
	//printf("malloc: %d\n",sizeof(Param));
	next->next = NULL;

	if(head == NULL){

		head = next;

	}else{

		Param* append = head;

		while(append->next != NULL){

			append = append->next;
		}

		append->next = next;
	}

}

Param* getParam(int no){

	Param* selected = head;
	while(no > 0){

		selected = selected->next;
		no--;
	}

	return selected;
}

struct FunctionInfo{

	int Size;
	char* name;
	int endOfList;
	char* parentFunction;
	struct FunctionInfo* nextFunction;
};

typedef struct FunctionInfo FunctionInfo;

FunctionInfo* newFunctionInfo();

FunctionInfo* funcHead = NULL;
int maxParams;

//calculates the memory allocation for each function
//and the maximum number of parameters
void calculateFunctionInfo(struct TacLine* lines){

	struct TacLine* next = lines;
	FunctionInfo* current;
	maxParams = 0;
	int currentFuncParams = 0;
	int functionCount = 0;

	while(next != NULL){

		if(next->operator == '=' && next->variable[0] != '$'
				&& next->isVariableCreation){
			//assignment
			current->Size += 4;

		//argument
		}else if(next->operator == 'A'){

			current->Size += 4;
			currentFuncParams ++;

		//function call
		}else if(next->operator == 'F'){

			current->Size += 8;

		//if and while
		}else if(next->operator == 'C' || next->operator == 'W'){

			current->Size += 4;

			if(next->thereIsElse){

				current->Size += 4;
			}

		//allocate for new function
		}else if(next->operator == 'D'){

			char* context = NULL;
			if(functionCount > 0){

				context = current->name;
			}

			current = newFunctionInfo(context);
			functionCount++;
			current->name = next->variable;
			current->Size = 8;
			//did the last function have the most number of parameters
			if(currentFuncParams > maxParams){

				maxParams = currentFuncParams;
			}
			currentFuncParams = 0;

		}else if(next->operator == 'E'){

			functionCount --;
		}

		next = next->next;
	}

}

//what is the parent of a function
char* getParent(char* functionName){

	FunctionInfo* next = funcHead;

	while(next != NULL){

		if(strcmp(next->name,functionName) == 0){

			return next->parentFunction;
		}

		next = next->nextFunction;
	}

	return NULL;
}

//does the function has a parent
int hasParent(char* functionName){

	return getParent(functionName) != NULL;
}

int getMaxParams(){

	return maxParams;
}

void printFunctionInfo(){

	FunctionInfo* next = funcHead;

	while(next != NULL){

		if(next == NULL){

			break;
		}

		printf("Function Info: %s %d \n",next->name,next->Size);

		next = next->nextFunction;
	}
}

//create new structure that hold information about functions
FunctionInfo* newFunctionInfo(char* context){

	FunctionInfo* new = (FunctionInfo*)malloc(sizeof(FunctionInfo));
	new->nextFunction = NULL;
	new->endOfList = 1;
	new->parentFunction = context;

	if(funcHead == NULL){

		funcHead = new;

	}else{

		FunctionInfo* append = funcHead;
		while(!append->endOfList){

			append = append->nextFunction;
		}

		append->endOfList = 0;
		append->nextFunction = new;
	}

	return new;
}


int getBytesToAllocation(char* symbol){

	FunctionInfo* next = funcHead;

	while(1){

		if(next->name == symbol){

			return next->Size;
		}

		if(next->nextFunction == NULL){

			break;
		}

		next = next->nextFunction;
	}

	return 0;
}




Value addNextMemLoc(char* symbol,int isVariableCreation,int* closureNo){

	if(isVariableCreation){
		Value value = getLastValue();
		value.isFunction = 0;
		value.valueType.intValue += 4;
		addSymbol(symbol,value,isVariableCreation);
		return value;
	}else{
		//need to update values in closure
		return getValueByEquality(symbol,closureNo);
	}


}

int isGlobalFunction(char* symbol){

	return isOnGlobalFrame(symbol);
}

//add the function name to the frame
void addFunctonToFrame(char* functionName){

	Value value;
	value.isFunction = 1;
	value.valueType.intValue = -4;
	addSymbol(functionName,value,1);
}


