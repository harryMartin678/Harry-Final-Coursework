/*
 * interpret.c
 *
 *  Created on: 6 Oct 2016
 *      Author: harry
 */

#include <stdio.h>
#include <ctype.h>
//#include "nodes.h"
#include "C.tab.h"
#include <string.h>
#include "frames.h"
#include <stdlib.h>

void parseParameters(NODE* parameter,NODE* argument,int backTrack);
int interpret(NODE* tree);
Value interpret0(NODE* tree,int* answerBranch);
Value evalFunction(NODE* tree,int backTrack);
Value evalExp0(NODE* tree,int backTrack);
Value evalExp(NODE* tree);
Value backTrackEvalExp(NODE* tree,int backTrack);
int evalCondition(NODE* tree);

int interpret(NODE* tree){

	printf("\n\n");
	int* answerBranch = (int*)malloc(sizeof(int));

	pushStack(NULL,"main");
	Value value = interpret0(tree,answerBranch);
	free(answerBranch);
	popStack();

	return value.valueType.intValue;
}

Value interpret0(NODE* tree,int* answerBranch){

	//printf("type: %s \n",named(tree->type));
	Value found;

	if(tree->type == 'D' ){

		TOKEN* function = ((TOKEN*)tree->left->right->left->left);

		if(strcmp(function->lexeme,"main") != 0){

			Value functionVal;
			Closure* closure = (struct Closure*)malloc(sizeof(struct Closure));
			//closure->env = getEnvironment();
			//printf("Env: %s For Function: %s\n",closure->env->functionName,function->lexeme);
			closure->functionBody = tree;
			functionVal.valueType.closure = closure;
			functionVal.isFunction = 1;
			//value.functionBody = (TOKEN*)tree;
			addSymbol(function->lexeme,functionVal);
			*answerBranch = 0;
			Value ret;
			ret.isFunction = 0;
			ret.valueType.intValue = 0;
			return ret;
		}

	}

	if(tree->type == RETURN){

		*answerBranch = 1;
		Value value = evalExp(tree);
		return value;


	}else if(tree->type == '='){

		Value evalValue = evalExp(tree->right);

		addSymbol(((TOKEN*)tree->left->left)->lexeme,evalValue);

		return evalValue;

	}else if(tree->type == IF){

		if(evalCondition(tree->left)){

			return interpret0(tree->right,answerBranch);

		}else if(tree->right->type == ELSE){

			return interpret0(tree->right->right,answerBranch);

		}else{

			*answerBranch = 0;
			Value value;
			value.isFunction = 0;
			value.valueType.intValue = 0;
			return value;
		}

	}else if(tree->type == WHILE){

		NODE* condition = tree->left;
		NODE* loopCode = tree->right;
		while(evalCondition(condition)){

			interpret0(loopCode,answerBranch);
		}

		*answerBranch = 0;

		Value zero;
		zero.isFunction = 0;
		zero.valueType.intValue = 0;

		return zero;

	}else if(tree->type != LEAF){

		*answerBranch = 0;
		int* leftBranch = (int*)malloc(sizeof(int));
		*leftBranch = 0;

		if(tree->left != NULL){

			Value leftAnswer = interpret0(tree->left,leftBranch);

			if(*leftBranch){

				found = leftAnswer;
				*answerBranch = 1;
				free(leftBranch);

			}
		}

		if(tree->right != NULL && *leftBranch != 1){

			int* rightBranch = (int*)malloc(sizeof(int));
			Value rightAnswer = interpret0(tree->right,rightBranch);

			if(*rightBranch){

				found = rightAnswer;
				*answerBranch = 1;
				free(rightBranch);
			}
		}



	}

	return found;

}

Value evalFunction(NODE* tree,int backTrack){

	Value functionVal;
	char* functionName = ((TOKEN*)tree->left->left)->lexeme;
	if(tree->left->type == APPLY){

		printf("apply %s\n",((TOKEN*)tree->left->right->left)->lexeme);
		functionVal = evalFunction(tree->left,backTrack);

	}else{

		printf("don't apply %s\n",functionName);
		functionVal = getValue(functionName);

	}

	Closure* function = functionVal.valueType.closure;

	printf("Env: %s, for function: %s\n",getEnvironment()->functionName,functionName);
	pushStack(getEnvironment(),((TOKEN*)tree->left->left->left)->lexeme);
	NODE* functionBody = function->functionBody;
	if(functionBody->left->right->right != NULL){
		parseParameters(functionBody->left->right->right,tree->right,backTrack);
	}

	int* answerBranch = (int*)malloc(sizeof(int));
	Value retValue = interpret0(functionBody->right,answerBranch);
	popStack();
	return retValue;

}

struct Parameter{

	char* symbol;
	struct Parameter *last;
};

void parseParameters(NODE* parameter,NODE* argument,int backtrack){

	if(parameter->type == ','){

		parseParameters(parameter->left,argument->left,backtrack);
		parseParameters(parameter->right,argument->right,backtrack);

	}else{

		TOKEN* parToken = (TOKEN*)parameter->right->left;
		Value value = backTrackEvalExp(argument,backtrack+1); //backTrack+1
		addSymbol(parToken->lexeme,value);

	}
}



Value evalExp0(NODE* tree,int backTrack){

	printf("eval Exp backTrack: %d\n",backTrack);
	Value zero;
	zero.isFunction = 0;
	zero.valueType.intValue = 0;
	if(tree == NULL) return zero;
	if(tree->type == APPLY) return evalFunction(tree,backTrack);
	printf("not function call\n");
	if(tree->type == LEAF){

		TOKEN* leaf = (TOKEN*)tree->left;
		if(leaf->type == IDENTIFIER){

			Value value;
			if(backTrack > 0){
				value = backTrackValue(leaf->lexeme,backTrack);
			}else{
				value = getValue(leaf->lexeme);
			}

			return value;


		}else{

			Value value;
			value.isFunction = 0;
			value.valueType.intValue = ((TOKEN*)tree->left)->value;
			return value;
		}

	}else{

		int valueLeft = evalExp0(tree->left,backTrack).valueType.intValue;
		int valueRight = evalExp0(tree->right,backTrack).valueType.intValue;
		int finalValue;

		if(tree->type == '+'){

			finalValue = valueLeft + valueRight;

		}else if(tree->type == '-'){

			finalValue = valueLeft - valueRight;


		}else if(tree->type == '*'){

			finalValue = valueLeft * valueRight;

		}else if(tree->type == '/'){

			finalValue = valueLeft / valueRight;

		}else{

			if(valueLeft != 0){

				finalValue = valueLeft;

			}else{

				finalValue = 0;

			}
		}

		Value ret;
		ret.isFunction = 0;
		ret.valueType.intValue = finalValue;

		return ret;
	}

}

Value evalExp(NODE* tree){

	return evalExp0(tree,0);
}

Value backTrackEvalExp(NODE* tree,int backTrack){

	return evalExp0(tree,backTrack);
}

int evalCondition(NODE* tree){

	if(tree->type == APPLY) return evalFunction(tree,0).valueType.intValue;

	if(tree->type == EQ_OP){

		return evalExp(tree->left).valueType.intValue == evalExp(tree->right).valueType.intValue;

	}else if(tree->type == NE_OP){

		return evalExp(tree->left).valueType.intValue != evalExp(tree->right).valueType.intValue;

	}else if(tree->type == LE_OP){

		return evalExp(tree->left).valueType.intValue <= evalExp(tree->right).valueType.intValue;

	}else if(tree->type == GE_OP){

		return evalExp(tree->left).valueType.intValue >= evalExp(tree->right).valueType.intValue;
	}

	return ((TOKEN*)tree->left)->value;

}


