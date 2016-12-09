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

struct Parameter{

	char* symbol;
	Value value;
	struct Parameter *last;
	struct Parameter *next;
};

typedef struct Parameter Parameter;

void parseParameters(NODE* parameter,NODE* argument,Parameter** nextParam);
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

	Value found;

	if(tree->type == 'D' ){

		TOKEN* function = ((TOKEN*)tree->left->right->left->left);

		if(strcmp(function->lexeme,"main") != 0){

			Value functionVal;
			Closure* closure = (struct Closure*)malloc(sizeof(struct Closure));
			closure->env = getEnvironment();

			closure->functionBody = tree;
			functionVal.valueType.closure = closure;
			functionVal.isFunction = 1;
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

		*answerBranch = 0;
		addSymbol(((TOKEN*)tree->left->left)->lexeme,evalValue);

		return evalValue;

	}else if(tree->type == IF){

		pushStack(getEnvironment(),"");
		if(evalCondition(tree->left)){

			Value ret = interpret0(tree->right->left,answerBranch);
			popStack();
			return ret;

		}else if(tree->right->type == ELSE){

			Value ret = interpret0(tree->right->right,answerBranch);
			popStack();
			return ret;

		}else{

			*answerBranch = 0;
			Value value;
			value.isFunction = 0;
			value.valueType.intValue = 0;
			popStack();
			return value;
		}


	}else if(tree->type == WHILE){

		NODE* condition = tree->left;
		NODE* loopCode = tree->right;
		pushStack(getEnvironment(),"");
		while(evalCondition(condition)){

			interpret0(loopCode,answerBranch);
		}
		popStack();
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

		functionVal = evalFunction(tree->left,backTrack);

	}else{

		functionVal = getValue(functionName);

	}

	Closure* function = functionVal.valueType.closure;

	Parameter** paramList = (Parameter**)malloc(sizeof(Parameter*));
	*paramList = NULL;
	NODE* functionBody = function->functionBody;
	if(functionBody->left->right->right != NULL){
		parseParameters(functionBody->left->right->right,tree->right,paramList);
	}

	pushStack(function->env,((TOKEN*)tree->left->left->left)->lexeme);

	while((*paramList) != NULL){

		addSymbol((*paramList)->symbol,(*paramList)->value);
		*paramList = (*paramList)->last;
	}

	int* answerBranch = (int*)malloc(sizeof(int));
	Value retValue = interpret0(functionBody->right,answerBranch);
	popStack();
	return retValue;

}




void parseParameters(NODE* parameter,NODE* argument,Parameter** paramList){

	if(parameter->type == ','){

		parseParameters(parameter->left,argument->left,paramList);
		parseParameters(parameter->right,argument->right,paramList);

	}else{

		TOKEN* parToken = (TOKEN*)parameter->right->left;
		Value value = evalExp(argument);
		if(*paramList == NULL){
			*paramList = (Parameter*)malloc(sizeof(Parameter));
			(*paramList)->last = NULL;
		}else{

			(*paramList)->next = (Parameter*)malloc(sizeof(Parameter));
			(*paramList)->next->last = *paramList;
			*paramList = (*paramList)->next;

		}

		(*paramList)->symbol = parToken->lexeme;
		(*paramList)->value = value;

	}
}



Value evalExp0(NODE* tree,int backTrack){

	Value zero;
	zero.isFunction = 0;
	zero.valueType.intValue = 0;
	if(tree == NULL) return zero;
	if(tree->type == APPLY) return evalFunction(tree,backTrack);
	if(tree->type == LEAF){

		TOKEN* leaf = (TOKEN*)tree->left;
		if(leaf->type == IDENTIFIER){

			Value value;
			value = getValue(leaf->lexeme);

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

	}else if(tree->type == '<'){

		return evalExp(tree->left).valueType.intValue < evalExp(tree->right).valueType.intValue;

	}else if(tree->type == '>'){

		return evalExp(tree->left).valueType.intValue > evalExp(tree->right).valueType.intValue;
	}

	return ((TOKEN*)tree->left)->value;

}


