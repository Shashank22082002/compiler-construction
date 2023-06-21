/*

//////////////////////////TEAM//////////////////////////
BITS ID             Member Name
2020A7PS0073P       Shashank Agrawal
2020A7PS0096P       Akshat Gupta
2020A7PS0129P       Anish Ghule
2020A7PS0134P       Shadan Hussain
2020A7PS1513P       Tarak P V S

*/

#ifndef CODEGEN_H
#define CODEGEN_H

#include<stdio.h>
#include "ast.h"
#include "symbolTable.h"

void generateExitAndErrorLabels(FILE *fp);
char *generateLabel();
void generateExpressionCode(astNode *root, symbolTable *currTable, symbolTable *globalTable, FILE *fp);
void generateCode(astNode *root, symbolTable *currTable, symbolTable *globalTable, FILE *fp);
void zeroAllRegisters(FILE *fp);
void generateExpressionCodeForModuleCall(astNode *root, symbolTable *currTable, symbolTable *globalTable, FILE *fp);
void generateExpressionCode(astNode *root, symbolTable *currTable, symbolTable *globalTable, FILE *fp);
void popAllRegisters(FILE *fp);
void pushAllRegisters(FILE *fp);
void generateRecursivePop(astNode* output_listNode, symbolTable* currTable, symbolTable* globalTable, FILE* fp);
char* generateEndLabel();
void pushAllRegisters(FILE* fp);
void popAllRegisters(FILE* fp);
void generateStartCode(FILE* fp);
void generateEndCode(FILE *fp);
void generateCode(astNode *root, symbolTable *currTable, symbolTable *globalTable, FILE *fp);
void generateFinalCode(char *input, char *output);

# endif /* codegen_h */