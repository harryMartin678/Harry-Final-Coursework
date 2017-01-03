#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
//#include "nodes.h"
//#include "C.tab.h"
#include "C.tab.h"
#include "interpret.h"
#include <string.h>

char *named(int t)
{
    static char b[100];
    if (isgraph(t) || t==' ') {
      sprintf(b, "%c", t);
      return b;
    }
    switch (t) {
      default: return "???";
    case IDENTIFIER:
      return "id";
    case CONSTANT:
      return "constant";
    case STRING_LITERAL:
      return "string";
    case LE_OP:
      return "<=";
    case GE_OP:
      return ">=";
    case EQ_OP:
      return "==";
    case NE_OP:
      return "!=";
    case EXTERN:
      return "extern";
    case AUTO:
      return "auto";
    case INT:
      return "int";
    case VOID:
      return "void";
    case APPLY:
      return "apply";
    case LEAF:
      return "leaf";
    case IF:
      return "if";
    case ELSE:
      return "else";
    case WHILE:
      return "while";
    case CONTINUE:
      return "continue";
    case BREAK:
      return "break";
    case RETURN:
      return "return";
    }
}

void print_leaf(NODE *tree, int level)
{
	//printf("Type leaf: %c \n",named(tree->type));
    TOKEN *t = (TOKEN *)tree;
    int i;
    for(i=0; i<level; i++) putchar(' ');//putchar(i+'1');
    if (t->type == CONSTANT) printf("Value: %d\n", t->value);
    else if (t->type == STRING_LITERAL) printf("STRING_LITERAL: \"%s\"\n", t->lexeme);
    else if (t){

    	printf("Puts: %s Type: %s\n",t->lexeme,named(tree->type));
    	//puts(t->lexeme);

    }
}

void print_tree0(NODE *tree, int level)
{
    int i;
    if (tree==NULL) return;
    if (tree->type==LEAF) {
      print_leaf(tree->left, level);
    }
    else {
      for(i=0; i<level; i++) putchar(' ');//putchar(i+'1');
      printf("Type: %s\n", named(tree->type));
/*       if (tree->type=='~') { */
/*         for(i=0; i<level+2; i++) putchar(' '); */
/*         printf("%p\n", tree->left); */
/*       } */
/*       else */

      	//for(i=0; i<level; i++) putchar(i+'1');
      	//printf("LEFT: %s\n", named(tree->type));
        print_tree0(tree->left, level+2);
        //for(i=0; i<level; i++) putchar(i+'1');
        //printf("RIGHT: %s\n", named(tree->type));
        print_tree0(tree->right, level+2);
    }
}

void print_tree(NODE *tree)
{
    print_tree0(tree, 0);
}


extern int yydebug;
extern NODE* yyparse(void);
extern NODE* ans;
extern void init_symbtable(void);

int main(int argc, char** argv)
{
    NODE* tree;
    if(argc < 2){

    	printf("Too few arguments");
    }
    char* option = argv[1];
    if (argc>2 && strcmp(argv[2],"-d")==0) yydebug = 1;
    init_symbtable();

    yyparse();
    tree = ans;

    printf("\n\n");

    if(strcmp(option,"INT") == 0){

    	printf("--C INTEPRETER\n");
    	printf("Answer: %d\n",interpret(tree,0));

    }else if(strcmp(option,"CMP") == 0){

    	printf("--C COMPILER\n");
    	compileToAssembly(tree,0);

    }else if(strcmp(option, "TAC") == 0){

    	printf("--C TAC ONLY COMPILER\n");
    	compile(tree);

    }else if(strcmp(option,"CMPOPT") == 0){

    	printf("--C OPTIMIZE COMPILER\n");
    	compileToAssembly(tree,1);

    }else if(strcmp(option,"TREE") == 0){

    	printf("PRINT TREE\n");
    	print_tree(tree);

    }else{

    	printf("Invalid option.");
    }

    return 0;
}
