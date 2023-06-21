/*

//////////////////////////TEAM//////////////////////////
BITS ID             Member Name
2020A7PS0073P       Shashank Agrawal
2020A7PS0096P       Akshat Gupta
2020A7PS0129P       Anish Ghule
2020A7PS0134P       Shadan Hussain
2020A7PS1513P       Tarak P V S

*/


#ifndef PARSER_H
#define PARSER_H

#include "linkedList.h"
#include "parseTree.h"
#include <limits.h>
#include <stdio.h>
#include<stdlib.h>
#include "lexer.h"
#include "hashmap.h"



bool isTerminal(char *str);

bool isNullable(int varTokenNumber, int epsilonTokenNo, bool* rulesChecked, int* nullables, Linkedlist* grammarRules[], int totalRules);

int* FindNullables(Linkedlist* grammarRules[], int totalRules, int total_vars, int epsilonTokenNo);

Linkedlist* computeFirstSet(int varTokenNo, Linkedlist* grammarRules[], Linkedlist** firstSets, int totalRules, bool* rulesChecked, int epsilonTokenNo);

Linkedlist** generateFirstSet(Linkedlist* grammarRules[], int totalRules, int total_vars, int epsilonTokenNo);

Linkedlist* computeFollowSet(int varTokenNo, Linkedlist* grammarRules[], Linkedlist** followSet, int totalRules, int total_vars, int epsilonTokenNo, Linkedlist** firstSets, int *nullables);

Linkedlist** generateFollowSet(Linkedlist* grammarRules[], int totalRules, int total_vars, int epsilonTokenNo, Linkedlist** firstSets, int *nullables);

int** createParseTable(int token_no, int totalRules, int epsilonTokenNo, Linkedlist *grammarRules[], HashmapEntry variables[], char *reverseMap[], int *nullables, Linkedlist** firstSets, Linkedlist** followSets);

ParseTree* readGrammar (char* output, FILE* fp);

void parseTheCode(char* input, char* output);

void printExecutionTime(char* output, char * codefilename);

void initMappings();

void freeMappings();

void printOnConsole(char *output);

#endif /* PARSER_H */