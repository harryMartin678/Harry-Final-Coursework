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

		}else if(next->operator == 'A'){

			current->Size += 4;
			currentFuncParams ++;

		}else if(next->isRegisterFunctionCall){

			current->Size += 4;

		}else if(next->operator == 'C' || next->operator == 'W'){

			current->Size += 4;

			if(next->thereIsElse){

				current->Size += 4;
			}

		}else if(next->operator == 'D'){

			char* context = NULL;
			if(functionCount > 0){

				context = current->name;
			}

			current = newFunctionInfo(context);
			functionCount++;
			current->name = next->variable;
			current->Size = 8;
			if(currentFuncParams > maxParams){

				maxParams = currentFuncParams;
			}
			currentFuncParams = 0;

		}else if(next->operator == 'E'){

			functionCount --;
		}

		next = next->next;
	}
	//printFunctionInfo();

}

char* getParent(char* functionName){

	FunctionInfo* next = funcHead;

	while(next != NULL){

		if(strcmp(next->name,functionName) == 0){

			//printf("hasParent: a:{%s} b:{%s} c:{%s}\n",functionName,next->name,next->parentFunction);
			return next->parentFunction;
		}

		next = next->nextFunction;
	}

	return NULL;
}

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

FunctionInfo* newFunctionInfo(char* context){

	FunctionInfo* new = (FunctionInfo*)malloc(sizeof(FunctionInfo));
	//printf("malloc: %d\n",sizeof(FunctionInfo));
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


/*int currentOffset;

void setMemoryOffset(int offset){

	currentOffset = offset;
}*/

Value addNextMemLoc(char* symbol,int isVariableCreation,int* closureNo){

	if(isVariableCreation){
		Value value = getLastValue();
		//printf("value: %d\n",value.valueType.intValue);
		value.isFunction = 0;
		value.valueType.intValue += 4;
		//printf("Symbol %s Value: %d\n",symbol,value.valueType.intValue);
		//if(value.valueType.intValue == 4){
		//	value.valueType.intValue += currentOffset;
		//}
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

void addFunctonToFrame(char* functionName){

	Value value;
	value.isFunction = 1;
	value.valueType.intValue = -4;
	addSymbol(functionName,value,1);
}


