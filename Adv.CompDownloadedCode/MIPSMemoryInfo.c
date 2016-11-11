#include "frames.h"
#include "TacLineQueue.h"
#include <stdio.h>


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

	while(next != NULL){

		if(next->operator == '=' && next->variable[0] != '$'){
			//assignment
			current->Size += 4;

		}else if(next->operator == 'A'){

			current->Size += 4;
			maxParams ++;

		}else if(next->operator == 'D'){

			current = newFunctionInfo();
			current->name = next->variable;
			current->Size = 8;
		}

		next = next->next;
	}

}

int getMaxParams(){

	return maxParams;
}

void printFunctionInfo(){

	FunctionInfo* next = funcHead;

	while(next != NULL){

		printf("%s %d \n",next->name,next->Size);

		if(next->endOfList){

			break;
		}

		next = next->nextFunction;
	}
}

FunctionInfo* newFunctionInfo(){

	FunctionInfo* new = (FunctionInfo*)malloc(sizeof(FunctionInfo*));
	new->endOfList = 1;

	if(funcHead == NULL){

		funcHead = new;

	}else{

		FunctionInfo* append = funcHead;
		while(!append->endOfList){

			append = append->nextFunction;
		}

		append->nextFunction = new;
		append->endOfList = 0;
	}

	return new;
}


int getBytesToAllocation(char* symbol){

	FunctionInfo* next = funcHead;

	while(1){

		if(next->name == symbol){

			return next->Size;
		}

		if(next->endOfList){

			break;
		}

		next = next->nextFunction;
	}

	return 0;
}


int currentOffset;

void setMemoryOffset(int offset){

	currentOffset = offset;
}


union Value addNextMemLoc(char* symbol){

	union Value value = getLastValue();
	value.intValue += 4;
	if(value.intValue == 4){
		value.intValue += currentOffset;
	}
	addSymbol(symbol,value);

	return value;
}


