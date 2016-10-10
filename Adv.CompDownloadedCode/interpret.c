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

int foundReturn;
char* test;


void testFunc(){


}

int interpret(NODE* tree){

	int* answerBranch = (int)malloc(sizeof(int));

	int value = interpret0(tree,answerBranch);
	free(answerBranch);

	return value;
}

int interpret0(NODE* tree,int* answerBranch){

	int found = 0;

	if(strcmp(named(tree->type),"D") == 0){

		TOKEN* function = ((TOKEN*)tree->left->right->left->left);

		if(strcmp(function->lexeme,"main") != 0){

			printf("Created Function: %s \n",function->lexeme);
			function-> next = (TOKEN*)tree;

			test = function->lexeme;
			//TOKEN* functionCpy = lookup_token(function->lexeme);
			//printf("EQUAL: %d %d\n",functionCpy->next == NULL,function-> next == NULL);
		}


	}

	if(tree->type == RETURN){

		*answerBranch = 1;

		return evalExp(tree);


	}else if(tree->type == '='){

		int evalValue = evalExp(tree->right);

		//dTOKEN* variable = lookup_token(((TOKEN*)tree->left->left)->lexeme);
		((TOKEN*)tree->left->left)->value = evalValue;

		return evalValue;

	}else if(tree->type == IF){

		if(evalCondition(tree->left)){

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

	printf("ENTER EVAL FUNCTION %s \n",((TOKEN*)tree->left->left)->lexeme);
	TOKEN* functionBody = (TOKEN*)lookup_token(((TOKEN*)tree->left->left)->lexeme);

	printf("TOKEN NEXT: %d\n",test == ((TOKEN*)tree->left->left)->lexeme);

	return 0;

}

int evalExp(NODE* tree){


	if(tree == NULL) return 0;
	if(tree->type == APPLY) return evalFunction(tree);

	if(tree->type == LEAF){

		TOKEN* leaf = (TOKEN*)tree->left;

		if(leaf->type == IDENTIFIER){

			TOKEN* variable = lookup_token(leaf->lexeme);
			return variable->value;
		}

		return ((TOKEN*)tree->left)->value;

	}else{

		int valueLeft = evalExp(tree->left);
		int valueRight = evalExp(tree->right);

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


