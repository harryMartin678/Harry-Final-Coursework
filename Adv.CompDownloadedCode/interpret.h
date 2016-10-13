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

	char* line;
	struct TacLine* nextLine;
};

int interpret(NODE* tree,int level);
void testFunc();


struct TacLine* compile(NODE* tree);
void test_func();

#endif /* INTERPRET_H_ */
