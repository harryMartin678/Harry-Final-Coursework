/*
 * frame.h
 *
 *  Created on: 12 Oct 2016
 *      Author: harry
 */

union Value{

	int intValue;
	NODE* functionBody;
};
void pushStack();
void popStack();
void addSymbol(char* symbol,union Value value);
union Value getValue(char* symbol);
union Value backTrackValue(char* symbol);
