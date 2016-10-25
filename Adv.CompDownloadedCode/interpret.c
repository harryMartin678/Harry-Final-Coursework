/*
 * interpret.c
 *
 *  Created on: 6 Oct 2016
 *      Author: harry
 */

#include <stdio.h>
#include <ctype.h>
#include "nodes.h"
#include "C.tab.h"
#include <string.h>
#include "frames.h"



void testFunc(){

	pushStack();
    union Value value1;
    value1.intValue = 2;
	addSymbol("x",value1);
	union Value value2;
	value2.intValue = 5;
	addSymbol("y",value2);
	pushStack();
	union Value value3;
	value3.intValue = 1;
	addSymbol("x",value3);

	union Value ret = getValue("x");
	printf("%d\n",ret.intValue);
}

int interpret(NODE* tree){

	int* answerBranch = (int)malloc(sizeof(int));

	pushStack();
	int value = interpret0(tree,answerBranch);
	free(answerBranch);
	popStack();

	return value;
}

int interpret0(NODE* tree,int* answerBranch){

	//printf("type: %s\n",named(tree->type));
	int found = 0;

	if(strcmp(named(tree->type),"D") == 0){

		TOKEN* function = ((TOKEN*)tree->left->right->left->left);

		if(strcmp(function->lexeme,"main") != 0){

			//printf("Created Function: %s \n",function->lexeme);
			function-> next = (TOKEN*)tree;
			*answerBranch = 0;
			return 0;
			//TOKEN* functionCpy = lookup_token(function->lexeme);
			//printf("EQUAL: %d %d\n",functionCpy->next == NULL,function-> next == NULL);
		}


	}

	if(tree->type == RETURN){

		*answerBranch = 1;

		int value = evalExp(tree);
		return value;


	}else if(tree->type == '='){


		int evalValue = evalExp(tree->right);

		//dTOKEN* variable = lookup_token(((TOKEN*)tree->left->left)->lexeme);
		//((TOKEN*)tree->left->left)->value = evalValue;
		union Value value;
		value.intValue = evalValue;
		addSymbol(((TOKEN*)tree->left->left)->lexeme,value);

		return evalValue;

	}else if(tree->type == IF){

		if(evalCondition(tree->left,answerBranch)){

			return interpret0(tree->right->left,answerBranch);

		}else if(tree->right->type == ELSE){

			return interpret0(tree->right->right,answerBranch);
		}

	}else if(tree->type != LEAF){


		*answerBranch = 0;

		if(tree->left != NULL){

			int* leftBranch = (int)malloc(sizeof(int));
			int leftAnswer = interpret0(tree->left,leftBranch);

			if(leftBranch){

				found = leftAnswer;
				*answerBranch = 1;
				free(leftBranch);
			}
		}

		if(tree->right != NULL){

			int* rightBranch = (int)malloc(sizeof(int));
			int rightAnswer = interpret0(tree->right,rightBranch);

			if(rightBranch){

				found = rightAnswer;
				*answerBranch = 1;
				free(rightBranch);
			}
		}

	}

	return found;

}

int evalFunction(NODE* tree){

	TOKEN* functionBody = (TOKEN*)lookup_token(((TOKEN*)tree->left->left)->lexeme);
	NODE* body = (NODE*) functionBody->next;
	pushStack();
	parseParameters(body->left->right->right,tree->right);

	int* answerBranch = (int*)malloc(sizeof(int));
	int retValue = interpret0(body->right,answerBranch);
	popStack();
	return retValue;

}

struct Parameter{

	char* symbol;
	struct Parameter *last;
};

void parseParameters(NODE* parameter,NODE* argument){

	if(parameter->type == ','){

		parseParameters(parameter->left,argument->left);
		parseParameters(parameter->right,argument->right);

	}else{

		TOKEN* parToken = (TOKEN*)parameter->right->left;
		union Value value;
		value.intValue = backTrackEvalExp(argument);
		addSymbol(parToken->lexeme,value);

	}
}



int evalExp0(NODE* tree,int backtrack){

	if(tree == NULL) return 0;
	if(tree->type == APPLY) return evalFunction(tree);

	if(tree->type == LEAF){

		TOKEN* leaf = (TOKEN*)tree->left;
		if(leaf->type == IDENTIFIER){

			if(backtrack){
				return backTrackValue(leaf->lexeme).intValue;
			}else{
				return getValue(leaf->lexeme).intValue;
			}


		}else{

			return ((TOKEN*)tree->left)->value;
		}

	}else{

		int valueLeft = evalExp0(tree->left,backtrack);
		int valueRight = evalExp0(tree->right,backtrack);

		if(tree->type == '+'){

			return valueLeft + valueRight;

		}else if(tree->type == '-'){

			return valueLeft - valueRight;


		}else if(tree->type == '*'){

			return valueLeft * valueRight;

		}else if(tree->type == '/'){

			return valueLeft / valueRight;

		}else{

			if(valueLeft != 0){

				return valueLeft;

			}else{

				return 0;

			}
		}
	}

}

int evalExp(NODE* tree){

	return evalExp0(tree,0);
}

int backTrackEvalExp(NODE* tree){

	return evalExp0(tree,1);
}

int evalCondition(NODE* tree){

	if(tree->type == APPLY) return evalFunction(tree);

	if(tree->type == EQ_OP){

		return evalExp(tree->left) == evalExp(tree->right);

	}else if(tree->type == NE_OP){

		return evalExp(tree->left) != evalExp(tree->right);

	}else if(tree->type == LE_OP){

		return evalExp(tree->left) <= evalExp(tree->right);

	}else if(tree->type == GE_OP){

		return evalExp(tree->left) >= evalExp(tree->right);
	}

	return ((TOKEN*)tree->left)->value;

}


