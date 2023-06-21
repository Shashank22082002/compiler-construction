/*

//////////////////////////TEAM//////////////////////////
BITS ID             Member Name
2020A7PS0073P       Shashank Agrawal
2020A7PS0096P       Akshat Gupta
2020A7PS0129P       Anish Ghule
2020A7PS0134P       Shadan Hussain
2020A7PS1513P       Tarak P V S

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "linkedList.h"
#include "lexer.h"
#include "parseTree.h"
#include "hashmap.h"
#include "global.h"

///////////////////ParseTreeNode Functions///////////////////////////
ParseTree *createNewParseTree()
{
    ParseTree *newTree = (ParseTree *)malloc(sizeof(ParseTree));
    newTree->root = NULL;
    return newTree;
}

ParseTreeNode *createNewParseTreeNode(tokenInfo *token, ParseTreeNode *parent, int variableNo, int RuleNo)
{
    ParseTreeNode *newNode = (ParseTreeNode *)malloc(sizeof(ParseTreeNode));
    newNode->token = token;
    newNode->variableNo = variableNo;
    newNode->ruleNo = RuleNo;
    newNode->children = (childrenList *)malloc(sizeof(childrenList));
    newNode->children->count = 0;
    newNode->children->head = NULL;
    newNode->sibling = NULL;
    newNode->parent = parent;
    return newNode;
}

void addChild(ParseTreeNode *parent, ParseTreeNode *child)
{

    // add child to the last of the children list
    if (parent->children->count == 0)
    {
        parent->children->head = child;
    }
    else
    {
        ParseTreeNode *temp = parent->children->head;
        child->sibling = temp;
        parent->children->head = child;
    }
    parent->children->count++;
    child->parent = parent;
}

///////////////////Stack Functions///////////////////////////
stack *createStack()
{
    stack *s = (stack *)malloc(sizeof(stack));
    s->size = 0;
    s->head = NULL;
    return s;
}

int isEmpty(stack *s)
{
    return s->size == 0;
}

void push(stack *s, ParseTreeNode *treeNode)
{
    stackNode *node = (stackNode *)malloc(sizeof(stackNode));
    node->treeNode = treeNode;
    node->next = s->head;
    s->head = node;
    s->size++;
}

ParseTreeNode *pop(stack *s)
{
    if (isEmpty(s))
    {
        printf("Stack is empty");
        return NULL;
    }
    stackNode *node = s->head;
    ParseTreeNode *treeNode = node->treeNode;
    s->head = node->next;
    free(node);
    s->size--;
    return treeNode;
}

ParseTreeNode *peek(stack *s)
{
    if (isEmpty(s))
    {
        printf("Stack is empty");
        return NULL;
    }
    return s->head->treeNode;
}

///////////////////Other Functions///////////////////////////
bool isTerminalToken(int tokenNo)
{
    if (tokenNo >= 0 && tokenNo <= DOLLAR)
        return true;
    return false;
}

Linkedlist *findSyncSet(int variableNo, Linkedlist **followSets, Linkedlist **firstSets, int *nullables)
{
    Linkedlist *syncSet = createNewList();
    // add follow of variableNo to syncSet
    Node *followIterator = followSets[variableNo]->head;
    while (followIterator)
    {
        insertNode(syncSet, followIterator->value, followIterator->isTerminal);
        followIterator = followIterator->next;
    }
    return syncSet;
}

ParseTree *generateParseTree(int **parseTable, Linkedlist **firstSets, Linkedlist **followSets, Linkedlist **grammarRules, HashmapEntry *variables, char *reverseMap[], int *nullables, int rule_no, int token_no, int epsilonTokenNo, FILE *fp)
{
    printf("\033[32mPARSING STARTED\033[0m\n");
    stack *st = createStack();
    ParseTree *parseTree = createNewParseTree();
    ParseTreeNode *curr = createNewParseTreeNode(NULL, NULL, search(variables, "<program'>"), -1);
    parseTree->root = curr;
    push(st, curr);
    tokenInfo *token = getNextToken(fp);
    bool NoErrorsDetected = 1;
    while (!isEmpty(st))
    {
        ParseTreeNode *top = peek(st);
        // printf("Current token is %s Stack top is %s\n", getTokenName(token->tokenType), reverseMap[top->variableNo]);
        // printf("Stack top: %d: ", top->variableNo);
        if (isTerminalToken(top->variableNo))
        {
            if (top->variableNo == token->tokenType)
            {
                top->token = token;
                // printf("Matched %s\n", getTokenName(token->tokenType));
                pop(st);
                if (noTokensLeft)
                    break;
                token = getNextToken(fp);
                // printf("Next token is %s\n", getTokenName(token->tokenType));
            }
            else
            {
                // printf("TODO HEURISTICS\n");
                NoErrorsDetected = 0;
                __SYNTAX_ERROR__ = true;
                printf("\033[31mSYNTAX ERROR: TERMINAL MISMATCH \033[0m");
                printf("\033[32mExpecting: %s, Got: %s, Line No: %d\033[0m\n", reverseMap[top->variableNo], getTokenName(token->tokenType), token->lineNo);
                bool reachedConsistentState = false;
                pop(st); // popped stack top... now continue as normal algorithm
                // printf("Popping stack top %s\n", reverseMap[top->variableNo]);
                // printf("Starting again\n");
                // exit(69);
                // TODO HEURISTICS
            }
        }
        else if (top->variableNo == epsilonTokenNo)
        {
            // printf("Matched %s\n", "epsilon");
            pop(st);
        }
        else
        {
            // printf("row  = %d, col = %d\n", top->variableNo - epsilonTokenNo - 1, token->tokenType);
            int ruleNo = parseTable[top->variableNo - epsilonTokenNo - 1][token->tokenType];
            // printf("Current Rule No is %d \n", ruleNo);
            if (ruleNo == -1)
            {
                // printf("variable %s, token %s\n", reverseMap[top->variableNo], getTokenName(token->tokenType));
                // printf("TODO HEURISTICS\n");
                NoErrorsDetected = 0;
                __SYNTAX_ERROR__ = true;
                printf("\033[31mSYNTAX ERROR: NON TERMINAL MISMATCH ON LINE NO %d\033[0m\n", token->lineNo);
                Linkedlist *syncSet = findSyncSet(top->variableNo, followSets, firstSets, nullables);
                // search for token in sync set till token is available
                while (searchNode(syncSet, token->tokenType) == false && !noTokensLeft)
                {
                    // printf("Skipping token %s\n", getTokenName(token->tokenType));
                    token = getNextToken(fp);
                }
                if (noTokensLeft)
                {
                    break;
                }
                // printf("\033[32mFound token %s in sync set, Restarting!\033[0m\n", getTokenName(token->tokenType));
                pop(st);
                // exit(69);
                // TODO HEURISTICS
                // make a sync set for the current variable include follow set if it is nullable else the first set

                // skip tokens untill a token in sync set is matched or current token is semicol
                // if semicol is found and not matched till now then check from default sync set comprising of
            }
            else
            {
                // pop the non terminal at the top of stack and push all rhs variables in reverse order on the stack
                top->ruleNo = ruleNo;
                pop(st);
                Linkedlist *rule = grammarRules[ruleNo];
                Node *temp = rule->head->next;
                stack *tempStack = createStack();
                while (temp != NULL)
                {
                    int variableNo = temp->value;
                    ParseTreeNode *newNode = createNewParseTreeNode(NULL, top, variableNo, -1);
                    push(tempStack, newNode);
                    temp = temp->next;
                }
                while (!isEmpty(tempStack))
                {
                    ParseTreeNode *newNode = pop(tempStack);
                    push(st, newNode);
                    addChild(top, newNode);
                }
            }
        }
    }
    if (!isEmpty(st))
    {
        NoErrorsDetected = 0;
        __SYNTAX_ERROR__ = true;
        printf("\033[31mERROR: INPUT IS CONSUMED BUT PARSING STACK IS NOT EMPTY\033[0m\n");
    }
    if (noTokensLeft)
        printf("\033[32mPARSING IS COMPLETED\033[0m\n");
    else
    {
        NoErrorsDetected = 0;
        __SYNTAX_ERROR__ = true;
        printf("\033[31mERROR: INPUT IS NOT CONSUMED BUT PARSING STACK IS EMPTY\033[0m\n");
    }

    if (NoErrorsDetected)
    {
        printf("\033[32mInput Source Code is Syntactically Correct......\033[0m\n");
    }
    else
    {
        printf("\033[32mAbove Shown Errors Occurred....\033[0m\n");
    }
    return parseTree;
}

void printInorder(ParseTreeNode *root, FILE **parseTreeFile, char *reverseMap[])
{
    if (root == NULL)
        return;
    ParseTreeNode *node = root->children->head;
    printInorder(node, parseTreeFile, reverseMap);
    // printf("%s\t", reverseMap[root->variableNo]);
    // lexeme CurrentNode lineno tokenName valueIfNumber  parentNodeSymbol isLeafNode(yes/no) NodeSymbol
    char *lexeme = "----";
    if (root->token)
        lexeme = root->token->lexeme;
    else if (root->variableNo == DOLLAR + 1)
        lexeme = "epsilon";
    char lineNo[21] = "";
    if (root->token)
        sprintf(lineNo, "%d", root->token->lineNo);
    char valueIfNumber[21];
    char *tokenName = reverseMap[root->variableNo];
    if (root->token && root->token->tokenType == NUM)
        sprintf(valueIfNumber, "%d", root->token->tokenData.ifInt);
    else if (root->token && root->token->tokenType == RNUM)
        sprintf(valueIfNumber, "%f", root->token->tokenData.ifFloat);
    else if (root->token && root->token->tokenType == TRUE)
        strcpy(valueIfNumber, "TRUE");
    else if (root->token && root->token->tokenType == FALSE)
        strcpy(valueIfNumber, "FALSE");
    else
        strcpy(valueIfNumber, "");
    char *parentNodeSymbol = ((root->parent) != NULL) ? reverseMap[root->parent->variableNo] : "ROOT";
    char *isLeafNode = (root->children->count == 0) ? "yes" : "no";
    char *nodeSymbol = reverseMap[root->variableNo];
    fprintf(*parseTreeFile, "%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s\n", lexeme, lineNo, tokenName, valueIfNumber, parentNodeSymbol, isLeafNode, nodeSymbol);
    for (int i = 0; i < 210; i++)
        fprintf(*parseTreeFile, "_");
    fprintf(*parseTreeFile, "\n");
    if (node == NULL)
        return;
    while ((node = node->sibling) != NULL)
        printInorder(node, parseTreeFile, reverseMap);
}

void printParseTree(ParseTree *parseTree, char *outfile, char *reverseMap[])
{
    FILE **parseTreeFile = (FILE **)malloc(sizeof(FILE *));
    *parseTreeFile = fopen(outfile, "w");
    char *heading[7] = {" lexeme", " lineNo", " tokenName", " valueIfNumber", " parentNodeSymbol", " isLeafNode", " nodeSymbol"};
    fprintf(*parseTreeFile, "%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s\n", heading[0], heading[1], heading[2], heading[3], heading[4], heading[5], heading[6]);
    for (int i = 0; i < 210; i++)
        fprintf(*parseTreeFile, "_");
    fprintf(*parseTreeFile, "\n");
    if (parseTreeFile == NULL)
    {
        printf("Error opening parse Tree File\n");
        return;
    }
    printInorder(parseTree->root, parseTreeFile, reverseMap);
    fclose(*parseTreeFile);
}
