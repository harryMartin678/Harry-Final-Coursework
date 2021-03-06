/*
 * MIPSMemoryInfo.h
 *
 *  Created on: 9 Nov 2016
 *      Author: harry
 */

#ifndef MIPSMEMORYINFO_H_
#define MIPSMEMORYINFO_H_

#include "frames.h"
#include <stdio.h>

struct Param{

	int number;
	int memoryPos;
	int value;
	struct Param* next;
};
typedef struct Param Param;
void addParam(int value);
Param* getParam(int no);
void calculateFunctionInfo(struct TacLine* lines);
int getMaxParams();
void printFunctionInfo();
Value addNextMemLoc(char* symbol,int isVariableCreation,int closureNo);
int getBytesToAllocation(char* symbol);
//void setMemoryOffset(int offset);
int hasParent(char* functionName);
char* getParent(char* functionName);
void addFunctonToFrame(char* functionName);
int isGlobalFunction(char* symbol);
#endif /* MIPSMEMORYINFO_H_ */
