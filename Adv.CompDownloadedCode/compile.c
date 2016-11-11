/*
 * compile.c
 *
 *  Created on: 12 Oct 2016
 *      Author: harry
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
//#include "nodes.h"
//#include "C.tab.h"
#include "TacLineQueue.h"
#include "MIPSMemoryInfo.h"

//TacLine{

	//int variable;
	//int operand1;
	//int isVar1;
	//int operand2;
	//int isVar2;
//	char operator;
//};

typedef struct TacLine TacLine;

int temp_count = 0;

char* MAINSTRING = "main";
char* IFSTRING = "IF";

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

void test_func(){

	int a = 10;
	char* combine;
	intToString("start",a,"end",&combine);
	printf("Combine: %s\n",combine);
}

struct TypeValue{

	int value;
	char* lexeme;
	int type;
};

void printVarAssignment(char* variable, char* variable2,char op){

	printf("%s %c %s;\n",variable,op,variable2);

}

void createVarAssignment(char* variable, char* variable2,char op,int isVar1Temp,int isVar2Temp){

	TacLine* tacline = (TacLine*)malloc(sizeof(TacLine));
	tacline->variable = variable;
	tacline->isVar1Temp = isVar1Temp;
	tacline->isVar2Temp = isVar2Temp;
	tacline->variable2 = variable2;
	tacline->operator = op;
	tacline->isNext = 0;
	tacline->isStatement = 0;
	tacline->isVariableEq = 1;
	tacline->isSimple = 1;
	tacline->next = NULL;

	addToQueue(tacline);
}

struct TypeValue IfVarWrap(struct TypeValue var){

	if(var.type == 3){

		int d = genTemp();
		char* temp;
		intToString("$t",d,"",&temp);
		printVarAssignment(temp,var.lexeme,'=');
		createVarAssignment(temp,var.lexeme,'=',1,0);

		struct TypeValue new;
		new.value = d;
		new.type = 1;
		return new;

	}else{

		return var;
	}
}

void printParam(struct TypeValue value){

	if(value.type == 1){

		printf("PushParam $t%d;\n",value.value);

	}else if(value.type == 3){

		printf("PushParam %s;\n",value.lexeme);
	}
}

void createParam(struct TypeValue value){

	struct TacLine* tacline = (struct TacLine*)malloc(sizeof(TacLine));
	tacline->operator = 'P';
	tacline->isNext = 0;
	tacline->isSimple = 0;
	tacline->isStatement = 0;
	tacline->isVariableEq = 0;
	tacline->paramType = value.type;

	if(value.type == 1){

		tacline->operand1 = value.value;
		tacline->isVar1 = 1;

	}else if(value.type == 3){

		tacline->variable = value.lexeme;
	}

	addToQueue(tacline);
}

void printPopArg(struct TypeValue value){

	printf("PopArg %s;\n",value.lexeme);

}

void createPopArg(struct TypeValue value){

	struct TacLine* line = (struct TacLine*)malloc(sizeof(struct TacLine));
	line->operator = 'A';
	line->variable = value.lexeme;
	line->paramType = value.type;

	addToQueue(line);
}

struct TypeValue placeInterInTemp(struct TypeValue value){

	if(value.type == 0){

		int temp = reuseTemp();
		char* var;
		intToString("$t",temp,"",&var);
		printSimpleAssignment(var,value.value,0);
		createSimpleInstruct(var,value.value,0,'=');

		struct TypeValue newValue;
		newValue.type = 1;
		newValue.value = temp;
		return newValue;

	}else{

		return value;
	}
}

void createFunctionCall(char* function){

	struct TacLine* line = (struct TacLine*)malloc(sizeof(struct TacLine));
	line->variable = function;
	line->operator = 'F';
	line->isStatement = 0;
	line->isNext = 0;
	line->isSimple = 0;
	line->next = NULL;

	addToQueue(line);
}


struct TypeValue compile0(NODE* tree,int tabs){

	//TacLine* tacLine = (TacLine*)malloc(sizeof(TacLine));

	//printf("Type: %s \n",named(tree->type));
	switch(tree->type){

	case 'D':

		//printf("%s \n",((TOKEN*)tree->left->right->left)->lexeme);
		//tacLine->line = ((TOKEN*)tree->left->right->left)->lexeme;
		//tacLine->line = "main:";
		resetTemp();
		TOKEN* name = (TOKEN*)tree->left->right->left->left;
		printf("%s: \n",name->lexeme);
		createStatement(name->lexeme);
		compile0(tree->left,tabs);
		compile0(tree->right,tabs+1);

		break;


	case ',':

		;
		if(tree->left->type != '~'){
			struct TypeValue valueL = compile0(tree->left,tabs);

			valueL = placeInterInTemp(valueL);
			printParam(valueL);
			createParam(valueL);

		}else{

			struct TypeValue valueL = compile0(tree->left->right,tabs);
			printPopArg(valueL);
			createPopArg(valueL);
		}

		if(tree->right->type != '~'){
			struct TypeValue valueR = compile0(tree->right,tabs);

			valueR = placeInterInTemp(valueR);
			printParam(valueR);
			createParam(valueR);

		}else{

			struct TypeValue valueR = compile0(tree->right->right,tabs);
			printPopArg(valueR);
			createPopArg(valueR);
		}
		break;

	case APPLY:

		genTemp();
		compile0(tree->right,tabs);
		TOKEN* call = (TOKEN*)tree->left->left;

		int temp1 = genTemp();
		printf("$t%d = LCall %s;\n",temp1,call->lexeme);
		createFunctionCall(call->lexeme);

		struct TypeValue funcBack;
		funcBack.type = 1;
		funcBack.value = temp1;

		return funcBack;

	case RETURN:
		{
			//int value = ((TOKEN*)tree->left)->value;
			struct TypeValue value = compile0(tree->left,tabs);

			//char[] line = new char[6+numDigits(value)];
			char* start = "Return ";

			//printf("RET TYPE: %d ",value.type);
			//printTabs(tabs);
			if(value.type == 0){

				printf("%s %d;\n",start,value.value);

			}else if(value.type == 1){

				printf("%s $t%d;\n",start,value.value);

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

		int t = genTemp();
		char* temp;
		intToString("$t",t,"",&temp);

		printSimpleAssignment(temp,a.value,a.type == 1);
		createSimpleInstruct(temp,a.value,a.type == 1,'=');
		TOKEN* variable = ((TOKEN*)tree->left->left);
		//printTabs(tabs);
		//printSimpleAssignment(temp,a.value,a.type == 1);
		//createSimpleInstruct(temp,a.value,a.type == 1,'=');
		printVarAssignment(variable->lexeme,temp,'=');
		createVarAssignment(variable->lexeme,temp,'=',0,1);

		break;

	case '+':
	case '*':
	case '-':
	case '/':
	case '<':
	case '>':
		//EquationState* state = parseEquation(tree,0);
		//printEquations(state);
		//printEquations(tree);
		//printTabs(tabs);
		;
		struct TypeValue l = compile0(tree->left,tabs);
		struct TypeValue r = compile0(tree->right,tabs);
		int d = genTemp();
		char* var;
		intToString("$t",d,"",&var);

		l = IfVarWrap(l);
		r = IfVarWrap(r);

		if(l.type == 0 && r.type == 0){

			int d2 = genTemp();
			char* inter;
			intToString("$t",d2,"",&inter);

			createSimpleInstruct(inter,l.value,0,'=');
			printSimpleAssignment(inter,l.value,0);

			struct TypeValue lv;
			lv.value = d2;
			lv.type = 1;

			printTacLine(var,tree->type,lv,r);
			createInstruction(var,lv.value,lv.type,r.value,r.type,tree->type);

		}else{

			printTacLine(var,tree->type,l,r);
			createInstruction(var,l.value,l.type,r.value,r.type,tree->type);
		}

		struct TypeValue tv;
		tv.value = d;
		tv.type = 1;
		return tv;

	case IF:
		;
		//printTabs(tabs);
		struct TypeValue condition = compile0(tree->left,tabs+1);
		printf("IF t%d:\n",condition.value);
		//createSimpleInstruct(IFSTRING,condition.value,condition.type == 1,'C');
		createInstruction(IFSTRING,condition.value,condition.type == 1,1,0,'C');
		compile0(tree->right,tabs+1);

		break;

	case ELSE:
		compile0(tree->left,tabs+1);
		printf("ELSE: \n");
		compile0(tree->right,tabs+1);
		break;
	case LEAF:

		if(tree->left->type == CONSTANT){
			struct TypeValue v;
			v.value = ((TOKEN*)tree->left)->value;
			v.type = 0;
			return v;

		}else if(tree->left->type == IDENTIFIER){

			struct TypeValue a;
			a.lexeme = ((TOKEN*)tree->left)->lexeme;
			a.type = 3;
			return a;

		}
		break;
	default:

		compile0(tree->left,tabs);
		if(tree->right != NULL){
			compile0(tree->right,tabs);
		}
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



void createStatement(char* statement){

	TacLine* line = (TacLine*)malloc(sizeof(TacLine));
	line->variable = statement;
	line->isStatement = 1;
	line->isVariableEq = 0;
	line->isNext = 0;
	line->operator = 'D';
	line->next = NULL;

	addToQueue(line);
}

void createSimpleInstruct(char* variable,int operand1,int isVar1,int op){

	TacLine* line = (TacLine*)malloc(sizeof(TacLine));
	line->variable = variable;
	line->operand1 = operand1;
	line->isVar1 = isVar1;
	line->isSimple = 1;
	line->isStatement = 0;
	line->isVariableEq = 0;
	line->operator = op;
	line->next = NULL;
	line->isNext = 0;

	addToQueue(line);
}

void createInstruction(char* variable,int operand1,int isVar1, int operand2,int isVar2, char operator){


	TacLine* line = (TacLine*)malloc(sizeof(TacLine));
	line->variable = variable;
	line->operand1 = operand1;
	line->isVar1 = isVar1;
	line->operand2 = operand2;
	line->isVar2 = isVar2;
	line->operator = operator;
	line->isSimple = 0;
	line->isStatement = 0;
	line->isVariableEq = 0;
	line->next = NULL;
	line->isNext = 0;

	if(!isVar1){

		int tempIsVar = line->isVar1;
		int tempOperand = line->operand1;

		line->isVar1 = line->isVar2;
		line->operand1 = line->operand2;

		line->isVar2 = tempIsVar;
		line->operand2 = tempOperand;
	}

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

void printTacLine(char* var,char op, struct TypeValue left, struct TypeValue right){

	//printf("t%d = ",var);
	printf("%s = ",var);

	printOperand(left,0);
	printf(" %c ",op);
	printOperand(right,1);

	printf(";\n");
}

void printOperand(struct TypeValue operand,int isRight){

	if(operand.type == 0){

		printf("%d",operand.value);

	}else if(operand.type == 1){

		printf("t%d",operand.value);

	}else if(operand.type == 3){

		printf("%s",operand.lexeme);
	}
}

void resetTemp(){

	temp_count = 0;
}

int genTemp(){

	return ++temp_count;
}

int reuseTemp(){

	return temp_count;
}


int strNumSize(int value,int strLength){

	int digitsNo = numDigits(value);
	return strLength * value * sizeof(char);

}

void intToString(char* start,int value,char* end,char** combine){

	int digitsNo = numDigits(value);
	char num[digitsNo];
	itoa(value,num,10);

	*combine = (char*)malloc(sizeof(char)*(digitsNo + strlen(start) + strlen(end)));
	strcpy(*combine,start);
	strcat(*combine,num);
	strcat(*combine,end);
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



void convertToAssembly(TacLine* line){

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
	case '<':
		instruct = "slt";
		break;
	case '>':
		instruct = "sgt";
		break;
	case '=':
		if(line->isVariableEq){

			if(line->isVar1Temp){

				instruct = "lw";

			}else{

				instruct = "sw";
			}


		}else{

			if(line->isVar1 && line->isVar2){

				instruct = "move";

			}else{

				instruct = "li";
			}


		}
		break;
	case 'C':
		instruct = "beq";
		break;

	case 'P':

		if(line->paramType == 3){

			printParamInstruct(line->variable,line->paramType);

		}else{

			printParamInstruct(&line->operand1,line->paramType);

		}
		break;

	case 'F':

		printFunctionCall(line->variable);

		break;
	}

	printAssemInstruct(instruct,line->variable,line->operand1,line->isVar1,line->operand2,line->isVar2,
			line->isSimple,line->isStatement,line->isVariableEq,line->variable2);

}

void printFunctionCall(char* function){

	printf("jal %s\n",function);
	pushStack();
}

void printParamInstruct(void* param,int type){

	if(type == 1){

		printf("subu ($sp), ($sp), 4 \n");
		printf("sw $t%d ($sp)\n",*((int*) param));

	}else{

		printf("subu ($sp), ($sp), 4\n");
		printf("lw $a1, ($sp)\n");
		printf("sw $a1, ($sp)\n");

	}


}

void printAssemInstruct(char* instruct,char* variable, int operand1, int isVar1, int operand2, int isVar2,
		int isSimple,int isStatement,int isVariableEq,char* variable2){

	printf("Assem Instruct: %s \n",instruct);
	if(isStatement){

		//variable
		if(strcmp(variable,MAINSTRING) == 0){

			printf(".text\n.globl\tmain\n");
			pushStack();
		}

		printf("%s:\n",variable);

		int bytesToAll = getBytesToAllocation(variable);

		printf("li $v0, 9\n");
		printf("li $a0,%d\n",bytesToAll);
		printf("syscall\n");
		printf("sw $ra, ($v0)\n");
		printf("sw $fp, 4($v0)\n");
		if(strcmp(variable,MAINSTRING) != 0){
			printf("lw $t0, 4($fp)\n");
			printf("sw $t0, 4($v0)\n");
			setMemoryOffset(8);
		}else{
			setMemoryOffset(4);
		}
		printf("move $fp, $v0");


	}else if(isVariableEq){


		char* stackPos;
		if(strcmp(instruct,"sw") == 0){

			int value = addNextMemLoc(variable).intValue;
			printf("%s %s %d($fp)",instruct,variable2,value);

		}else if(strcmp(instruct,"lw") == 0){

			int offset = getValueByEquality(variable2).intValue;
			intToString("",offset,"($fp)",&stackPos);
			printf("%s %s %s",instruct,variable,stackPos);
			free(stackPos);
		}


	}else{
		printf("%s ",instruct);
		if(variable != IFSTRING){
			printAssemOperand(variable,1,1);
		}
		printAssemOperand(&operand1,isVar1,0);
		if(!isSimple){

			printAssemOperand(&operand2,isVar2,0);
		}
	}
	printf("\n");

}


void printAssemOperand(void* operand,int isVar,int isStr){

	if(isVar && isStr){

		printf("%s ",operand);

	}else if(isVar){

		printf("$t%d ",*((int*)operand));

	}else{

		printf("%d ",*((int*)operand));
	}
}


void compile(NODE* tree){

	//TacLine* firstLine = (TacLine*)malloc(sizeof(TacLine));
	compile0(tree,0);
}

void createParamData(int numOfParams){

	if(numOfParams > 0){
		printf(".data\nparams:	.word ");
		int i;
		for(i = 0; i < numOfParams-1; i++)printf("0, ");
		printf("0\n");
	}

}


void compileToAssembly(NODE* tree){

	compile0(tree,0);

	printf("\n");

	calculateFunctionInfo(getElement(0));

	createParamData(getMaxParams());

	//printFunctionInfo();

	int i;
	for(i = 0; i < getSize(); i++){

		convertToAssembly(getElement(i));
	}

	printf("li $v0,10\n");
	printf("syscall\n");
	printf(".end main\n");

}



