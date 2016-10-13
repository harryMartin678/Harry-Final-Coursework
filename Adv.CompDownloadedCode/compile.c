/*
 * compile.c
 *
 *  Created on: 12 Oct 2016
 *      Author: harry
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "nodes.h"
#include "C.tab.h"


struct TacLine{

	char* line;
	struct TacLine* nextLine;
};

struct EquationState{

	char* variable;
	char* operand1;
	char* operand2;
	char* operator;
	struct EquationState* left;
	struct EquationState* right;
};


typedef struct TacLine TacLine;
typedef struct EquationState EquationState;

void test_func(){

	char* src = "start\0";
	char* dest = (char*)malloc(sizeof(char)*strlen(src));
	copy(src,dest);
	printf("%s \n",dest);

}

void copy(char* src,char* dest){

	strcpy(dest,src);
}

char* getOperand(NODE* tree){

	int value = ((TOKEN*)tree->left)->value;
	int digits = numDigits(value);
	char* num = (char*)malloc(sizeof(char)*digits);
	itoa(value,num,10);
	return num;
}

EquationState* parseEquation(NODE* tree){

	if(named(tree->type) == '+' && named(tree->type) == '-'
			&& named(tree->type) == '*' && named(tree->type) =='/'){

		EquationState* state = (EquationState*)malloc(sizeof(EquationState));
		state->operand1 = getOperand(tree->left);
		state->operand2 = getOperand(tree->right);
		state->operator = named(tree->type);
		state->variable = "x";
		state->left = NULL;
		state->right = NULL;

		return state;

	}else{

		EquationState* left = parseEquation(tree->left);
		EquationState* right = parseEquation(tree->right);

		EquationState* this = (EquationState*)malloc(sizeof(EquationState));
		this->left = left;
		this->right = right;

		return this;
	}
}

void printEquations(EquationState* next){

	printf("%s %s %s %s\n",next->operand1,next->operand2,next->operator,next->variable);

	if(next->left != NULL){

		printEquations(next->left);

	}else if(next->right != NULL){

		printEquations(next->right);
	}

}


void compile0(NODE* tree,struct TacLine* currentLine){

	struct TacLine* tacLine = (struct TacLine*)malloc(sizeof(struct TacLine));

	switch(tree->type){

	case 'D':

		//printf("%s \n",((TOKEN*)tree->left->right->left)->lexeme);
		//tacLine->line = ((TOKEN*)tree->left->right->left)->lexeme;
		tacLine->line = "main:";
		compile0(tree->right,tacLine);
		break;

	case RETURN:
		{
			int value = ((TOKEN*)tree->left)->value;
			//char[] line = new char[6+numDigits(value)];
			char* start = "\tRETURN \0";

			char* combine = (char*)malloc(strNumSize(value,strlen(start)));
			createNumEndLine(start,value,combine);
			tacLine->line = combine;
			tacLine->nextLine = NULL;
		}

		break;

	case '=':
		printf("= \n");
		EquationState* state = parseEquation(tree);
		printEquations(state);
		break;
	default:
		//printf("SET NULL\n");
		//free(tacLine);
		//tacLine = NULL;
		break;
	}

	currentLine->nextLine = tacLine;
}


int strNumSize(int value,int strLength){

	int digitsNo = numDigits(value);
	return strLength * value * sizeof(char);

}

void createNumEndLine(char* start,int value,char* combine){

	int digitsNo = numDigits(value);
	char num[digitsNo];
	itoa(value,num,10);

	strcpy(combine,start);
	strcat(combine,num);
	char* end = ";";
	strcat(combine,end);
}


int numDigits(int number)
{
    int digits = 0;
    if (number < 0) digits = 1; // remove this line if '-' counts as a digit
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}


struct TacLine* compile(NODE* tree){

	TacLine* firstLine = (TacLine*)malloc(sizeof(TacLine));
	compile0(tree,firstLine);

	return firstLine->nextLine;
}



