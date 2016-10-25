/*
 * interpret.h
 *
 *  Created on: 6 Oct 2016
 *      Author: harry
 */
#include "nodes.h"


#ifndef INTERPRET_H_
#define INTERPRET_H_

struct TacLine{

	int variable;
	int operand1;
	int isVar1;
	int operand2;
	int isVar2;
	char operator;
};

int interpret(NODE* tree,int level);


struct TacLine* compile(NODE* tree);
void compileToAssembly(NODE* tree);
void testfunc();

#endif /* INTERPRET_H_ */
