/*

//////////////////////////TEAM//////////////////////////
BITS ID             Member Name
2020A7PS0073P       Shashank Agrawal
2020A7PS0096P       Akshat Gupta
2020A7PS0129P       Anish Ghule
2020A7PS0134P       Shadan Hussain
2020A7PS1513P       Tarak P V S

*/
#ifndef GLOBAL_H
#define GLOBAL_H

#include<stdio.h>
#include "hashmap.h"
#include "lexer.h"
#include "symbolTable.h"

// GLOBALS
extern bool __SYNTAX_ERROR__ ;
extern bool __SEMANTIC_ERROR__; 
extern bool __RUNTIME_ERROR__ ;
extern bool readCodeCompletely; // set to true when fp reaches EOF
extern int lineNo;
extern bool noTokensLeft;
extern char *buff1, *buff2;
extern int forward, begin;
extern bool bufferFlush;
extern int lexLength;
extern bool lexLengthExceeded;
extern char ch;
extern bool isComment;
extern STATE current;
extern HashmapEntry *lookupTable;
extern int BUFF_SIZE;

extern char *NoToCharMapping[LIST_SIZE];
extern HashmapEntry CharToNoMapping[LIST_SIZE];

extern char *astOutput;
extern char *testfile;
extern char *treeOutput;
extern char *asmOutputFile;

extern ArrayDetails arrayList[100];
extern int arrayCount;


#endif /* GLOBAL_H */