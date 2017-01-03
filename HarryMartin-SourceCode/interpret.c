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

//represents a parameter in a parameter list
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
Value interpret1(NODE* tree,int* answerBranch,int variableCreated);
Value evalFunction(NODE* tree);
Value evalExp(NODE* tree);
int evalCondition(NODE* tree);

//start point
int interpret(NODE* tree){

	printf("\n\n");
	int* answerBranch = (int*)malloc(sizeof(int));

	//creates global frame where function definitions are stored
	pushStack(NULL,"main");
	Value value = interpret1(tree,answerBranch,0);
	free(answerBranch);
	//destroy global frame
	popStack();

	return value.valueType.intValue;
}

Value interpret0(NODE* tree,int* answerBranch){

	return interpret1(tree,answerBranch,0);
}

Value interpret1(NODE* tree,int* answerBranch,int variableCreated){

	Value found;

	//if the node is a function
	if(tree->type == 'D' ){

		TOKEN* function = ((TOKEN*)tree->left->right->left->left);

		//if it's not the main the store the environment and the code
		if(strcmp(function->lexeme,"main") != 0){

			Value functionVal;
			Closure* closure = (struct Closure*)malloc(sizeof(struct Closure));
			//create closure by taking the current environment(frame)
			closure->env = getEnvironment();

			closure->functionBody = tree;
			functionVal.valueType.closure = closure;
			functionVal.isFunction = 1;
			//add the function name to the frame so it can be recalled later
			addSymbol(function->lexeme,functionVal,1);
			*answerBranch = 0;
			Value ret;
			ret.isFunction = 0;
			ret.valueType.intValue = 0;
			return ret;

		}

	}

	if(tree->type == RETURN){

		//return the value of the return statement and set this branch to an answer branch
		*answerBranch = 1;
		Value value = evalExp(tree);
		return value;


	}else if(tree->type == '='){

		Value evalValue = evalExp(tree->right);

		*answerBranch = 0;
		//do an assignment
		//if a ~ was a parent of this node then this is a variable creation
		//variable creations are pushed on to this frame regardless if there is a variable
		//of the same name on the frame
		addSymbol(((TOKEN*)tree->left->left)->lexeme,evalValue,variableCreated);

		return evalValue;

	}else if(tree->type == IF){

		//create closure around the if statement
		pushStack(getEnvironment(),"");
		if(evalCondition(tree->left)){

			//if code is executed
			Value ret;
			//if there is no else code
			if(tree->right->type == ELSE){

				ret = interpret0(tree->right->left,answerBranch);

			}else{

				ret = interpret0(tree->right,answerBranch);
			}

			popStack();
			return ret;

		}else if(tree->right->type == ELSE){

			//execute else code
			Value ret = interpret0(tree->right->right,answerBranch);
			popStack();
			return ret;

		}else{

			//there is no else code and the if code was no executed
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
		//place closure around while loop
		pushStack(getEnvironment(),"");
		while(evalCondition(condition)){

			//interpret the while loop's code in till the condition is false
			Value possRet = interpret0(loopCode,answerBranch);

			//if there is a return statement inside the while statement
			if(*answerBranch == 1){

				return possRet;
			}
		}

		popStack();
		//there was no answer branch inside the while loop
		*answerBranch = 0;

		Value zero;
		zero.isFunction = 0;
		zero.valueType.intValue = 0;

		return zero;

	//if we need to keep tree walking
	}else if(tree->type != LEAF){

		*answerBranch = 0;
		int* leftBranch = (int*)malloc(sizeof(int));
		*leftBranch = 0;

		//check if there is an answer in the left branch
		if(tree->left != NULL){

			Value leftAnswer = interpret1(tree->left,leftBranch,tree->type == '~');

			if(*leftBranch){

				found = leftAnswer;
				*answerBranch = 1;
				free(leftBranch);

			}else{

				*answerBranch = 0;
			}
		}

		//if there wasn't an answer in the left branch then try the right branch
		if(tree->right != NULL && (*leftBranch) == 0){

			int* rightBranch = (int*)malloc(sizeof(int));
			Value rightAnswer = interpret1(tree->right,rightBranch,tree->type == '~');

			if(*rightBranch){

				found = rightAnswer;
				*answerBranch = 1;
				free(rightBranch);

			}else{

				*answerBranch = 0;
			}
		}

	}

	//return the answer in either the right or left branch if any
	return found;

}


//evaluate a function call
Value evalFunction(NODE* tree){

	Value functionVal;
	char* functionName = ((TOKEN*)tree->left->left)->lexeme;
	//if this is a function calling the results of another functions then call that function
	if(tree->left->type == APPLY){

		functionVal = evalFunction(tree->left);

	}else{

		//else get the function from the frame/closure
		functionVal = getValue(functionName);

	}

	Closure* function = functionVal.valueType.closure;

	//create list of parameters
	Parameter** paramList = (Parameter**)malloc(sizeof(Parameter*));
	*paramList = NULL;
	NODE* functionBody = function->functionBody;

	//find the parameters in the tree
	if(functionBody->left->right->right != NULL){
		parseParameters(functionBody->left->right->right,tree->right,paramList);
	}
	//create a new frame for this function
	pushStack(function->env,((TOKEN*)tree->left->left->left)->lexeme);
	//add the arguments to that frame
	while((*paramList) != NULL){

		addSymbol((*paramList)->symbol,(*paramList)->value,1);
		*paramList = (*paramList)->last;
	}

	//evaluate the function and return the result, as well as the answer branch
	int* answerBranch = (int*)malloc(sizeof(int));
	Value retValue = interpret0(functionBody->right,answerBranch);
	popStack();
	return retValue;

}


//collect the called function's parameters
void parseParameters(NODE* parameter,NODE* argument,Parameter** paramList){

	//keep looking for more parameters
	if(parameter->type == ','){

		parseParameters(parameter->left,argument->left,paramList);
		parseParameters(parameter->right,argument->right,paramList);

	}else{

		//add the parameter to the parameters list
		TOKEN* parToken = (TOKEN*)parameter->right->left;
		//print_tree(parameter);
		Value value = evalExp(argument);
		if(*paramList == NULL){

			*paramList = (Parameter*)malloc(sizeof(Parameter));
			(*paramList)->last = NULL;

		}else{

			//keeps a double linked list
			(*paramList)->next = (Parameter*)malloc(sizeof(Parameter));
			(*paramList)->next->last = *paramList;
			*paramList = (*paramList)->next;

		}

		(*paramList)->symbol = parToken->lexeme;
		(*paramList)->value = value;

	}
}

//evaluates an arithmetic expression
Value evalExp(NODE* tree){

	Value zero;
	zero.isFunction = 0;
	zero.valueType.intValue = 0;
	//if there is not assignment return zero
	if(tree == NULL) return zero;
	//if there is a function call, call it
	if(tree->type == APPLY) return evalFunction(tree);
	//if this is a leaf
	if(tree->type == LEAF){

		//either get the value of a variable
		TOKEN* leaf = (TOKEN*)tree->left;
		if(leaf->type == IDENTIFIER){

			Value value;
			value = getValue(leaf->lexeme);

			return value;

		//or get the a intermediate's value
		}else{

			Value value;
			value.isFunction = 0;
			value.valueType.intValue = ((TOKEN*)tree->left)->value;
			return value;
		}

	}else{

		//get the values from the left and right side of the tree
		int valueLeft = evalExp(tree->left).valueType.intValue;
		int valueRight = evalExp(tree->right).valueType.intValue;
		int finalValue;

		//do operations such as +, -, etc...
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

//evalulate a condition
int evalCondition(NODE* tree){

	//if there is function call then call it
	if(tree->type == APPLY) return evalFunction(tree).valueType.intValue;

	//evaluate the expression on both the left and right and do the operator on them
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


