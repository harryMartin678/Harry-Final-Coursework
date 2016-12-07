/*
 * TacLineQueue.h
 *
 *  Created on: 19 Oct 2016
 *      Author: harry
 */

#ifndef TACLINEQUEUE_H_
#define TACLINEQUEUE_H_

struct TacLine{

	char* variable;
	int paramType;
	int isVar1Temp;
	char* variable2;
	int isVar2Temp;
	int operand1;
	int isVar1;
	int operand2;
	int isVar2;
	int operator;
	int isSimple;
	int isStatement;
	int isVariableEq;
	int isNext;
	int isRegisterFunctionCall;
	int isVariableCreation;
	int thereIsElse;
	struct TacLine* next;
};

void addToQueue(struct TacLine* next);
struct TacLine* getElement(int index);
int getSize();

#endif /* TACLINEQUEUE_H_ */
