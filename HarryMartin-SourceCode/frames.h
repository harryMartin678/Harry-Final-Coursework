/*
 * frame.h
 *
 *  Created on: 12 Oct 2016
 *      Author: harry
 */

#include "nodes.h"
#include "C.tab.h"

struct Closure{

	NODE* functionBody;
	char* parentFunctionName;
	struct Frame* env;
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

struct Frame{

	struct SymbolNode* listHead;
	struct Frame* next;
	struct Frame* last;
	struct Frame* closure;
	char* functionName;
	int no;
};

typedef struct Frame Frame;

typedef union ValueType ValueType;
typedef struct Closure Closure;

void pushStack(struct Frame* env,char* functionName);
void popStack();
void addSymbol(char* symbol,Value value,int isVariableCreation);
void addSymbolByEquality(char* symbol, Value value,int isVariableCreation);
Frame* getEnvironment();
//void addSymbol0(char* symbol,Value value,int isClosure);
Value getValue(char* symbol);
void changeAllInFrame(int amount);
Value getValueByEquality(char* symbol,int* closureNo);
int containsSymbol(char* symbol);
Value getLastValue();
Value isFunctionPointer(char* symbol);
