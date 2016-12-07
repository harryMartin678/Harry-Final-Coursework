/*
 * compile.c
 *
 *  Created on: 12 Oct 2016
 *      Author: harry
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
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
int label_count = 0;

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

void printVarAssignment(char* variable, char* variable2,int op){

	printf("%s %c %s;\n",variable,op,variable2);

}

void createVarAssignment(char* variable, char* variable2,int op,int isVar1Temp,int isVar2Temp){

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
	tacline->isRegisterFunctionCall = 0;
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
	tacline->isRegisterFunctionCall = 0;
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

	TacLine* line = (struct TacLine*)malloc(sizeof(TacLine));
	line->operator = 'A';
	line->variable = value.lexeme;
	line->paramType = value.type;
	line->isRegisterFunctionCall = 0;

	addToQueue(line);
}

struct TypeValue placeInterInTemp(struct TypeValue value){

	//printf("Parameter lexeme: %s Type: %d\n",value.lexeme,value.type);
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

	}else if(value.type == 3){

		int temp = reuseTemp();
		char* var;
		intToString("$t",temp,"",&var);
		printVarAssignment(var,value.lexeme,'=');
		createVarAssignment(var,value.lexeme,'=',1,0);
		struct TypeValue newValue;
		newValue.type = 1;
		newValue.value = temp;
		return newValue;

	}else{

		return value;
	}
}

void createFunctionCall(char* function,int temp,int applyOnTemp){

	struct TacLine* line = (struct TacLine*)malloc(sizeof(struct TacLine));
	line->variable = function;
	line->operator = 'F';
	line->isStatement = 0;
	line->isNext = 0;
	line->isVar1Temp = 1;
	line->operand1 = temp;
	line->isSimple = 0;
	line->isRegisterFunctionCall = applyOnTemp;
	line->next = NULL;

	addToQueue(line);
}

void createStatement0(char* statement,int isFunction,char operator){

	TacLine* line = (TacLine*)malloc(sizeof(TacLine));
	line->variable = statement;
	line->isStatement = 1 + isFunction;
	line->isVariableEq = 0;
	line->isNext = 0;
	line->operator = operator;
	line->isRegisterFunctionCall = 0;
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
		//TOKEN* type = (TOKEN*)tree->left->left->left;
		//name->next = type;
		printf("%s: \n",name->lexeme);
		createStatement(name->lexeme);
		compile0(tree->left,tabs);
		compile0(tree->right,tabs+1);
		char* endState = (char*)malloc((strlen(".end ") + strlen(name->lexeme)) * sizeof(char));
		strcpy(endState,".end ");
		strcat(endState,name->lexeme);
		printf("%s\n",endState);
		createStatement0(endState,3,'E');
		break;

	case 'F':

		compile0(tree->left,tabs);
		if(tree->right != NULL){
			if(tree->right->type == ','){

				compile0(tree->right,tabs);

			}else{

				struct TypeValue arg = compile0(tree->right->right,tabs);
				printPopArg(arg);
				createPopArg(arg);
			}
		}

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

		;
		struct TypeValue onApply;
		int applyOnFunction = 0;
		if(tree->left->type == APPLY){

			onApply = compile0(tree->left,tabs);
			applyOnFunction = 1;
		}

		genTemp();
		if(tree->right != NULL){
			if(tree->right->type == ','){

				compile0(tree->right,tabs);

			}else{

				struct TypeValue param = compile0(tree->right,tabs);

				param = placeInterInTemp(param);
				printParam(param);
				createParam(param);
			}
		}

		TOKEN* call = (TOKEN*)tree->left->left;

		char* functionName;
		if(!applyOnFunction){

			functionName = call->lexeme;

		}else{

			intToString("$t",onApply.value,"",&functionName);
		}
		int temp1 = genTemp();
		printf("$t%d = LCall %s;\n",temp1,functionName);
		createFunctionCall(functionName,temp1,applyOnFunction);

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

			struct TacLine* line = (struct TacLine*)malloc(sizeof(struct TacLine));
			line->operator = 'R';
			line->isRegisterFunctionCall = 0;

			//printf("RET TYPE: %d ",value.type);
			//printTabs(tabs);
			if(value.type == 0){

				printf("%s %d;\n",start,value.value);
				line->operand1 = value.value;
				line->isVar1 = 0;

			}else if(value.type == 1){

				printf("%s $t%d;\n",start,value.value);
				line->operand1 = value.value;
				line->isVar1Temp = 1;
				line->isVar1 = 0;

			}else if(value.type == 3){

				printf("%s %s;\n",start,value.lexeme);
				line->variable = value.lexeme;
				line->isVar1 = 1;
			}

			line->next = NULL;
			addToQueue(line);
			line = NULL;

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
	case EQ_OP:
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
		if(condition.type == 0){

			printf("IF %d:\n",condition.value);

		}else if(condition.type == 1){

			printf("IF $t%d:\n",condition.value);

		}else{

			printf("IF %s:\n",condition.lexeme);
		}

		char* ifLabel;
		int labelC = genLabelCount();
		asprintf(&ifLabel,"if_%d",labelC);
		//createSimpleInstruct(IFSTRING,condition.value,condition.type == 1,'C');
		createInstruction(ifLabel,condition.value,condition.type == 1,1,0,'C');
		compile0(tree->right,tabs+1);
		//createStatement0(ifLabel,0);


		break;

	case ELSE:

		;
		char* endIf;
		asprintf(&endIf,"end_%d",getLabelCount());
		printf("ELSE: \n");
		compile0(tree->right,tabs+1);
		char* jumpToEnd = (char*)malloc(sizeof(char)*(strlen(endIf) + 4));
		strcpy(jumpToEnd,"jal ");
		strcat(jumpToEnd,endIf);
		createStatement0(jumpToEnd,2,'J');
		printf("THEN: \n");
		char* ifLabel2;
		asprintf(&ifLabel2,"if_%d",getLabelCount());
		createStatement0(ifLabel2,0,'J');
		compile0(tree->left,tabs+1);

		createStatement0(endIf,0,'J');
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

	createStatement0(statement,1,'D');
}

void createSimpleInstruct(char* variable,int operand1,int isVar1,int op){

	TacLine* line = (TacLine*)malloc(sizeof(TacLine));
	line->variable = variable;
	line->operand1 = operand1;
	line->isVar1 = isVar1;
	line->isSimple = 1;
	line->isStatement = 0;
	line->isVariableEq = 0;
	line->isRegisterFunctionCall = 0;
	line->operator = op;
	line->next = NULL;
	line->isNext = 0;

	addToQueue(line);
}

void createInstruction(char* variable,int operand1,int isVar1, int operand2,int isVar2, int operator){


	TacLine* line = (TacLine*)malloc(sizeof(TacLine));
	line->variable = variable;
	line->operand1 = operand1;
	line->isVar1 = isVar1;
	line->operand2 = operand2;
	line->isVar2 = isVar2;
	line->operator = operator;
	line->isRegisterFunctionCall = 0;
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

		printf("$t%d",value);

	}else{

		printf("%d",value);
	}

	printf(";\n");
}

void printTacLine(char* var,int op, struct TypeValue left, struct TypeValue right){

	//printf("t%d = ",var);
	printf("%s = ",var);

	printOperand(left,0);
	if(op == EQ_OP){

		printf(" == ");
	}else{

		printf(" %c ",op);
	}

	printOperand(right,1);

	printf(";\n");
}

void printOperand(struct TypeValue operand,int isRight){

	if(operand.type == 0){

		printf("%d",operand.value);

	}else if(operand.type == 1){

		printf("$t%d",operand.value);

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

int genLabelCount(){

	return ++label_count;
}

int getLabelCount(){

	return label_count;
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

struct FunctionNameNode{

	char* functionName;
	struct FunctionNameNode* next;
	struct FunctionNameNode* last;
};

typedef struct FunctionNameNode FunctionNameNode;

struct AssemblyContext{

	int paramNo;
	int isParam;
	//char* currentFunction;
	FunctionNameNode* head;
	FunctionNameNode* current;
};

typedef struct AssemblyContext AssemblyContext;


void pushFunctionName(char* functionName,AssemblyContext* context){

	if(context->head == NULL){

		context->head = (FunctionNameNode*)malloc(sizeof(FunctionNameNode));
		context->head->functionName = functionName;
		context->head->last = NULL;
		context->head->next = NULL;
		context->current = context->head;

	}else{

		FunctionNameNode* next = context->head;

		while(next->next != NULL){

			next = next->next;
		}

		next->next = (FunctionNameNode*)malloc(sizeof(FunctionNameNode));
		next->next->functionName = functionName;
		next->next->last = next;
		next->next->next = NULL;
		context->current = next->next;
	}
}

void popFunctionName(AssemblyContext* context){

	if(context->current != NULL){

		FunctionNameNode* cleanUp = context->current;

		if(cleanUp->last == NULL){

			context->head = NULL;
			context->current = NULL;

		}else{

			context->current = cleanUp->last;
		}

		free(cleanUp);
	}
}

/*struct AssemblyLine{

	char* line;
	struct AssemblyLine* next;
};

typedef struct AssemblyLine AssemblyLine;

AssemblyLine* assemblyHead = NULL;

void addAssemblyLine(char* line,...){

	va_list arg;
	int i;
	int nargs;

	for(i = 0; i < strlen(line); i++){

		if(line[i] == '%'){

			nargs++;
		}
	}

	for(i = 0; i < nargs; i++){


	}

	if(assemblyHead == NULL){

		assemblyHead = (AssemblyLine*)malloc(sizeof(AssemblyLine));
		assemblyHead->next = NULL;
		assemblyHead->line = line;

	}else{

		AssemblyLine* append = assemblyHead;

		while(append->next != NULL){

			append = append->next;
		}

		append->next = (AssemblyLine*)malloc(sizeof(AssemblyLine));
		append->next->line = line;
		append->next->next = NULL;
	}
}

void printAssembly(){

	AssemblyLine* next = assemblyHead;

	while(next != NULL){

		printf("%s",next->line);
	}
}*/


void convertToAssembly(TacLine* line,AssemblyContext* context){

	char* instruct;
	int hasPrinted = 0;

	if(line->operator != 'A' && !context->isParam){

		context->paramNo = 0;
	}

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
	case EQ_OP:
		instruct = "seq";
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

		printIfStatement(line);
		hasPrinted = 1;
		break;

	case 'P':

		if(line->paramType == 3){

			printParamInstruct(line->variable,line->paramType,context);

		}else{

			printParamInstruct(&line->operand1,line->paramType,context);

		}
		hasPrinted = 1;
		context->isParam = 1;
		context->paramNo++;
		break;

	case 'F':
		if(line->isRegisterFunctionCall){

			printFunctionCall(line->variable,line->operand1,context,1);

		}else{

			printFunctionCall(line->variable,line->operand1,context,0);
		}

		hasPrinted = 1;
		context->paramNo = 0;
		break;
	case 'A':
		printPopArgInstruct(line->variable,context);
		hasPrinted = 1;
		context->isParam = 0;
		context->paramNo++;
		break;
	case 'R':

		if(line->isVar1){

			printReturnStatementInstruct(line->variable,line->isVar1,line->isVar1Temp,context);

		}else{

			printReturnStatementInstruct(&line->operand1,line->isVar1,line->isVar1Temp,context);
		}

		hasPrinted = 1;
		break;
	}

	if(!hasPrinted){
		printAssemInstruct(instruct,line->variable,line->operand1,line->isVar1,line->operand2,line->isVar2,
				line->isSimple,line->isStatement,line->isVariableEq,line->variable2,context);
	}

}

void printLw(char* variable, char* variable2){

	int closureNo;
	Value offset = getValueByEquality(variable2,&closureNo);
	//printf("Enter lw two\n");
	//intToString("",offset,"($fp)",&stackPos);
	if(offset.isFunction == 0){
		if(closureNo > 0){

			printf("move $s0, $fp\n");
		}
		int i = closureNo;
		while(i-- > 0){

			printf("lw $s0, 4($s0)\n");
		}
		if(closureNo > 0){

			printf("lw %s %d($s0)",variable,offset.valueType.intValue);

		}else{

			printf("lw %s %d($fp)",variable,offset.valueType.intValue);
		}

	}else{

		printf("li $v0, 9\nli $a0, 8\nsyscall\n");
		printf("la $t0, %s\n",variable2);
		printf("sw $t0, ($v0)\n");
		printf("sw $fp, 4($v0)\n");
		printf("la %s, ($v0)",variable);
	}
}

void printIfStatement(TacLine* line){

	printf("beq ");
	printAssemOperand(&line->operand1,line->isVar1,0);
	printAssemOperand(&line->operand2,line->isVar2,0);
	printf("%s\n",line->variable);
}

void printReturnStatementInstruct(void* retValue, int isVar, int isTemp,AssemblyContext* context){

	if(isVar){

		//int closureNo;
		//int offset = getValueByEquality((char*)retValue,&closureNo).valueType.intValue;
		//char* newLine;
		//printf("lw $a0, %d($fp)\n",offset);
		printLw("$a0",(char*)retValue);
		printf("\n");

	}else if(isTemp){

		printf("move $a0, $t%d\n",*((int*)retValue));

	}else{

		printf("li $a0, %d\n",*((int*)retValue));
	}
	//if(context->currentFunction == MAINSTRING){
	//	printf("lw $ra, 0($fp)\n");
	//	printf("lw $fp, 4($fp)\n");
	//}

	printf("lw $ra, ($fp)\n");
	printf("lw $fp, 4($fp)\n");
//	if(strcmp(context->current->functionName,MAINSTRING) != 0){
		//printf("lw $s0, 8($fp)\n");
	//}
	printf("jr $ra\n");
}

void printPopArgInstruct(char* variable,AssemblyContext* context){

	//printf("arg variable: %s\n",variable);
	printf("lw $t0, %d($a1)\n",context->paramNo*4);
	printf("sw $t0,%d($fp)\n",(context->paramNo*4) + 8);
	setMemoryOffset(4);
	addNextMemLoc(variable);
}

void printFunctionCall(char* function,int temp,AssemblyContext* context,int callingTemp){

	char* parent = getParent(function);

	//if(parent != NULL && strcmp(parent,context->current->functionName) == 0){

		//printf("move $s1, $v0\n");
	//}

	if(callingTemp){

		printf("move $s2, $fp\n");
		printf("lw $fp, 4(%s)\n",function);
		printf("lw $t0, 0(%s)\n",function);
		printf("jalr $t0\n");
		printf("move $fp, $s2\n");

	}else{

		printf("jal %s\n",function);
	}

	printf("move $t%d, $a0\n",temp);
}

char* getFunctionNameFromEndTag(char* variable){

	int funcNameSize = (strlen(variable) - strlen(".end "))+1;
	char* functionName = (char*)malloc(funcNameSize * sizeof(char));

	int f = 0;
	for(int f = 0; f < funcNameSize-1; f++){

		functionName[f] = variable[f+strlen(".end ")];
	}

	functionName[funcNameSize-1] = '\0';

	return functionName;
}

void printParamInstruct(void* param,int type,AssemblyContext* context){

	if(context->paramNo == 0){
		printf("la $a1, params\n");
	}

	//printf("%s\n",param);
	printf("sw $t%d, %d($a1)\n",*((int*)param),context->paramNo*4);

}

void printAssemInstruct(char* instruct,char* variable, int operand1, int isVar1, int operand2, int isVar2,
		int isSimple,int isStatement,int isVariableEq,char* variable2,AssemblyContext* context){

	//printf("Assem Instruct: %s \n",instruct);
	if(isStatement){

		if(isStatement == 2){
			/*if(context->currentFunction != NULL){
				printf(".end %s\n",context->currentFunction);
			}*/

			//variable
			if(hasParent(variable)){

				printf("j end%s\n",variable);
				pushStack(getEnvironment(),"");
				addFunctonToFrame(variable);

			}else{

				pushStack(NULL,"");
			}

			//context->currentFunction = variable;
			pushFunctionName(variable,context);
			printf("%s:\n",variable);

			int bytesToAll = getBytesToAllocation(variable);

			printf("li $v0, 9\n");
			printf("li $a0,%d\n",bytesToAll);
			printf("syscall\n");
			printf("sw $ra, ($v0)\n");
			printf("sw $fp, 4($v0)\n");
			//if(strcmp(variable,MAINSTRING) != 0){
				//printf("sw $s0, 8($v0)\n");
				//printf("move $s0, $s1\n");
				//setMemoryOffset(12);
			//}else{
				setMemoryOffset(8);
			//}

			//if(strcmp(variable,MAINSTRING) != 0){

				//printf("lw $t0, 4($fp)\n");
				//printf("sw $t0, 4($v0)\n");


			//}
			printf("move $fp, $v0");

		}else{

			if(isStatement >= 3){

				printf("%s",variable);

				if(isStatement == 4){

					popFunctionName(context);
					char* functionName = getFunctionNameFromEndTag(variable);
					int needJump = hasParent(functionName);

					if(needJump){

						printf("\nend%s:",functionName);
					}
				}

			}else{

				printf("%s:",variable);
			}

		}

	}else if(isVariableEq){

		//char* stackPos;
		if(strcmp(instruct,"sw") == 0){

			int value = addNextMemLoc(variable).valueType.intValue;
			//printf("sw: %s %s %d($fp)",instruct,variable2,value);
			printf("%s %s %d($fp)",instruct,variable2,value);

		}else if(strcmp(instruct,"lw") == 0){

			printLw(variable,variable2);
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

	printf("\n\n\n");
	calculateFunctionInfo(getElement(0));
	createParamData(getMaxParams());
	//printFunctionInfo();

	printf(".text\n.globl\tmain\n");
	printf("_main:\njal main\nli $v0,10\nsyscall\n.end _main\n");
	AssemblyContext* context = (AssemblyContext*)malloc(sizeof(AssemblyContext));
	context->paramNo = 0;
	//context->currentFunction = NULL;
	int i;
	for(i = 0; i < getSize(); i++){

		convertToAssembly(getElement(i),context);
	}

	//printf("li $v0,10\n");
	//printf("syscall\n");
	//printf(".end main\n");

}



