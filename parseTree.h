/*

//////////////////////////TEAM//////////////////////////
BITS ID             Member Name
2020A7PS0073P       Shashank Agrawal
2020A7PS0096P       Akshat Gupta
2020A7PS0129P       Anish Ghule
2020A7PS0134P       Shadan Hussain
2020A7PS1513P       Tarak P V S

*/

#ifndef PARSETREE_H
#define PARSETREE_H


#include "lexer.h"

typedef struct ParseTreeNode
{
    tokenInfo *token; // Null for non terminals
    int variableNo;
    struct childrenList *children;
    struct ParseTreeNode *sibling;
    struct ParseTreeNode *parent;
    int ruleNo;
} ParseTreeNode;

typedef struct ParseTree
{
    ParseTreeNode *root;
} ParseTree;

typedef struct childrenList
{
    int count;
    struct ParseTreeNode *head;
} childrenList;

///////////////////Stack Data Structure///////////////////////////
typedef struct stackNode
{
    ParseTreeNode *treeNode;
    struct stackNode *next;
} stackNode;

typedef struct stack
{
    int size;
    stackNode *head;
} stack;

///////////////////Functions///////////////////////////
ParseTree *createNewParseTree();

ParseTreeNode *createNewParseTreeNode(tokenInfo *token, ParseTreeNode *parent, int variableNo, int RuleNo);

void addChild(ParseTreeNode *parent, ParseTreeNode *child);

stack *createStack();

int isEmpty(stack *s);

void push(stack *s, ParseTreeNode *treeNode);

ParseTreeNode *pop(stack *s);

ParseTreeNode *peek(stack *s);

bool isTerminalToken(int tokenNo);

Linkedlist *findSyncSet(int variableNo, Linkedlist **followSets, Linkedlist **firstSets, int *nullables);

ParseTree *generateParseTree(int **parseTable, Linkedlist **firstSets, Linkedlist **followSets, Linkedlist **grammarRules, HashmapEntry *variables, char *reverseMap[], int *nullables, int rule_no, int token_no, int epsilonTokenNo, FILE *fp);

void printInorder(ParseTreeNode *root, FILE **parseTreeFile, char *reverseMap[]);

void printParseTree(ParseTree *PT, char *outfile, char *reverseMap[]);

#endif /* PARSETREE_H */