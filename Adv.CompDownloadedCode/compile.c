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
#include "TacLineQueue.h"


//struct TacLine{

	//int variable;
	//int operand1;
	//int isVar1;
	//int operand2;
	//int isVar2;
//	char operator;
//};

typedef struct TacLine TacLine;

int temp_count = 0;

/*void test_func(){

	char* src = "start\0";
	char* dest = (char*)malloc(sizeof(char)*strlen(src));
	//copy(src,dest);
	printf("%s \n",dest);

}*/


void printEquations(NODE* tree){

	//printf("ENTER EQUATIONS %d\n",tree == NULL);
	if(tree == NULL) return;

	int recurse = 1;

	if(tree->type == LEAF){

		printf("Value: %d\n",((TOKEN*)tree->left)->value);
		recurse = 0;

	}else if(tree->type == '+'){

		printf("Operator: %s\n",named(tree->type));

	}

	if(recurse){

		printEquations(tree->left);
		printEquations(tree->right);
	}
}

struct TypeValue{

	int value;
	char* lexeme;
	int type;
};


struct TypeValue compile0(NODE* tree,int tabs){

	//printf("%s %d\n",named(tree->type),tabs);
	//struct TacLine* tacLine = (struct TacLine*)malloc(sizeof(struct TacLine));

	//printf("Type: %s \n",named(tree->type));
	switch(tree->type){

	case 'D':

		//printf("%s \n",((TOKEN*)tree->left->right->left)->lexeme);
		//tacLine->line = ((TOKEN*)tree->left->right->left)->lexeme;
		//tacLine->line = "main:";
		printf("main: \n");
		compile0(tree->right,tabs+1);
		break;

	case RETURN:
		{
			//int value = ((TOKEN*)tree->left)->value;
			struct TypeValue value = compile0(tree->left,tabs);

			//char[] line = new char[6+numDigits(value)];
			char* start = "RETURN \0";

			//printf("RET TYPE: %d ",value.type);
			//printTabs(tabs);
			if(value.type == 0){
				printf("%s %d;\n",start,value.value);

			}else if(value.type == 3){

				printf("%s %s;\n",start,value.lexeme);
			}

			//char* combine = (char*)malloc(strNumSize(value,strlen(start)));
			//createNumEndLine(start,value,combine);
			//tacLine->line = combine;
			//tacLine->leftLine = NULL;
			//tacLine->rightLine = NULL;
		}

		break;

	case '=':

		;
		struct TypeValue a = compile0(tree->right,tabs);
		TOKEN* variable = ((TOKEN*)tree->left->left);
		//printTabs(tabs);
		printSimpleAssignment(variable->lexeme,a.value,a.type == 1);
		createSimpleInstruct(variable->lexeme,a.value,a.type == 1);

		break;

	case '+':
	case '*':
	case '-':
	case '/':
	case '<':
	case '>':
		//printf("= \n");
		//EquationState* state = parseEquation(tree,0);
		//printEquations(state);
		//printEquations(tree);
		//printTabs(tabs);
		;
		struct TypeValue l = compile0(tree->left,tabs);
		struct TypeValue r = compile0(tree->right,tabs);
		int d = genTemp();
		printTacLine(d,tree->type,l,r);
		createInstruction(d,l.value,l.type,r.value,r.type,tree->type);
		struct TypeValue tv;
		tv.value = d;
		tv.type = 1;
		return tv;

	case IF:
		;
		//printTabs(tabs);
		struct TypeValue condition = compile0(tree->left,tabs+1);
		printf("IF t%d\n",condition.value);
		break;
	case LEAF:

		if(tree->left->type == CONSTANT){
			struct TypeValue v;
			v.value = ((TOKEN*)tree->left)->value;
			v.type = 0;
			return v;

		}else{

			struct TypeValue a;
			a.lexeme = ((TOKEN*)tree->left)->lexeme;
			a.type = 3;
			return a;
		}

	default:

		compile0(tree->left,tabs);
		compile0(tree->right,tabs);
	}

	//if(isLeft){
	//	currentLine->leftLine = tacLine;
	//}else{
		//currentLine->rightLine = tacLine;
	//}
	struct TypeValue n;
	n.value = 0;
	n.type = 2;
	return n;
}

void createSimpleInstruct(int variable,int operand1,int isVar1){

	struct TacLine* line = (struct TacLine*)malloc(sizeof(struct TacLine));
	line->variable = variable;
	line->operand1 = operand1;
	line->isVar1 = isVar1;
	line->isSimple = 1;
	line->operator = '=';
	line->next = NULL;

	addToQueue(line);
}

void createInstruction(int variable,int operand1,int isVar1, int operand2,int isVar2, char operator){

	struct TacLine* line = (struct TacLine*)malloc(sizeof(struct TacLine));
	line->variable = variable;
	line->operand1 = operand1;
	line->isVar1 = isVar1;
	line->operand2 = operand2;
	line->isVar2 = isVar2;
	line->operator = operator;
	line->isSimple = 0;
	line->next = NULL;

	addToQueue(line);
}

void printTabs(int tabs){

	for(int t = 0; t < tabs; t++)printf("\t");
}

void printSimpleAssignment(char* var, int value, int isVar){

	printf("%s = ",var);

	if(isVar){

		printf("t%d",value);

	}else{

		printf("%d",value);
	}

	printf(";\n");
}

void printTacLine(int var,char op, struct TypeValue left, struct TypeValue right){

	printf("t%d = ",var);

	printOperand(left);
	printf(" %c ",op);
	printOperand(right);

	printf(";\n");
}

void printOperand(struct TypeValue operand){

	if(operand.type == 1){

		printf("t%d",operand.value);

	}else if(operand.type == 0){

		printf("%d",operand.value);

	}else if(operand.type == 3){

		printf("%s",operand.lexeme);
	}
}

int genTemp(){

	return ++temp_count;
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


void convertToAssembly(struct TacLine* line){

	char* instruct;
	switch(line->operator){

	case '+':
		instruct = "add";
		break;
	case '-':
		instruct = "sub";
		break;
	case '*':
		instruct = "mul";
		break;
	case '/':
		instruct = "div";
		break;
	case '=':
		instruct = "load";
		break;
	}

	printAssemInstruct(instruct,line->variable,line->operand1,line->isVar1,line->operand2,line->isVar2,
			line->isSimple);
}

void printAssemInstruct(char* instruct,int variable, int operand1, int isVar1, int operand2, int isVar2,
		int isSimple){

	printf("%s ",instruct);
	printAssemOperand(variable,1);
	printAssemOperand(operand1,isVar1);
	if(!isSimple){
		printAssemOperand(operand2,isVar2);
	}
	printf("\n");

}

void printAssemOperand(int operand,int isVar){

	if(isVar){

		printf("$t%d ",operand);

	}else{

		printf("%d ",operand);
	}
}


struct TacLine* compile(NODE* tree){

	//TacLine* firstLine = (TacLine*)malloc(sizeof(TacLine));
	compile0(tree,0);

	return NULL;
}

void compileToAssembly(NODE* tree){

	compile0(tree,0);

	printf("\n");

	int i;
	for(i = 0; i < getSize(); i++){

		convertToAssembly(getElement(i));
	}
}



