/*

//////////////////////////TEAM//////////////////////////
BITS ID             Member Name
2020A7PS0073P       Shashank Agrawal
2020A7PS0096P       Akshat Gupta
2020A7PS0129P       Anish Ghule
2020A7PS0134P       Shadan Hussain
2020A7PS1513P       Tarak P V S

*/


#ifndef AST_H
#define AST_H

#include "parser.h"
#include "lexer.h"
#include "parseTree.h"
#include "linkedList.h"
#include<stdio.h>
#include<stdlib.h>

#define _INTEGER_WIDTH_ 2
#define _REAL_WIDTH_ 4
#define _BOOLEAN_WIDTH_ 1
#define _MIN_SP_SIZE_ 8
#define _ACT_INT_WIDTH_ 8
#define _ACT_REAL_WIDTH_ 8
#define _ACT_BOOLEAN_WIDTH_ 8



typedef struct SCOPE
{
    int start;
    int end;
} SCOPE;

typedef struct astNode
{
    tokenInfo *token;
    int variableNo;
    struct astNode *parent;
    struct astNode *children;
    struct astNode *sibling;
    SCOPE scope;
    bool ignore;
} astNode;

typedef struct AST
{
    struct astNode *root;
} AST;


astNode *createASTNode(tokenInfo *token, int variableNo);
void joinNodes(astNode **siblings, int count, astNode *parent);
astNode *addNodeToEnd(astNode *list, astNode *node, astNode *parent);
astNode *addNodeToBeg(astNode *list, astNode *node, astNode *parent);
astNode *applyASTRules(ParseTreeNode *parentNode);
AST *createAST(ParseTree *tree);
void printAST(astNode *root, FILE *fp, int noOfTabs);
void generateAST(AST* ast);
#endif /* AST_H */