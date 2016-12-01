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

struct Frame{

	struct SymbolNode* listHead;
	struct Frame* next;
	struct Frame* last;
	struct Frame* closure;
	char* functionName;
	int no;
};

typedef struct Frame Frame;

typedef struct Value Value;
typedef union ValueType ValueType;
typedef struct Closure Closure;

void pushStack(struct Frame* env,char* functionName);
void popStack();
void addSymbol(char* symbol,Value value);
Frame* getEnvironment();
//void addSymbol0(char* symbol,Value value,int isClosure);
Value getValue(char* symbol);
Value backTrackValue(char* symbol,int backTrack);
void changeAllInFrame(int amount);
Value getValueByEquality(char* symbol);
Value getLastValue();
