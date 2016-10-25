/*
 * TacLineQueue.h
 *
 *  Created on: 19 Oct 2016
 *      Author: harry
 */

#ifndef TACLINEQUEUE_H_
#define TACLINEQUEUE_H_

struct TacLine{

	int variable;
	int operand1;
	int isVar1;
	int operand2;
	int isVar2;
	char operator;
	int isSimple;
	struct TacLine* next;
};

void addToQueue(struct TacLine* next);
struct TacLine* getElement(int index);
int getSize();

#endif /* TACLINEQUEUE_H_ */
