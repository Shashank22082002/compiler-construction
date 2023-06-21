/*

//////////////////////////TEAM//////////////////////////
BITS ID             Member Name
2020A7PS0073P       Shashank Agrawal
2020A7PS0096P       Akshat Gupta
2020A7PS0129P       Anish Ghule
2020A7PS0134P       Shadan Hussain
2020A7PS1513P       Tarak P V S

*/

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "ast.h"
#include <stdbool.h>
#include <stdio.h>



typedef struct symbolTableEntry symbolTableEntry;
typedef struct symbolTable symbolTable;
typedef enum symbolTableEntryType
{
    OTHER_MODULE,
    DRIVER_MODULE,
    VARIABLE,
    CONDITIONAL_STMT,
    CASE_STMT,
    ITERATIVE_STMT
} symbolTableEntryType;

typedef enum PRIMITIVE_TYPE
{
    INTEGER_TYPE,
    REAL_TYPE,
    BOOLEAN_TYPE,
    INVALID_TYPE
} PRIMITIVE_TYPE;

typedef enum ARRAY_TYPE
{
    NOT_ARRAY,
    STATIC,
    DYNAMIC,
    LEFT_DYNAMIC,
    RIGHT_DYNAMIC

} ARRAY_TYPE;

typedef enum LOOP_TYPE
{
    FOR_LOOP,
    WHILE_LOOP
} LOOP_TYPE;

typedef struct DATA_TYPE
{
    ARRAY_TYPE arrType;
    PRIMITIVE_TYPE primitiveType;
    int leftBound; // if array, stores left bound. If bound is dynamic it stores sign
    int rightBound;
    char *leftID;
    char *rightID;
} DATA_TYPE;

typedef struct DATA_TYPE_NODE
{
    // for list in module i/o list
    DATA_TYPE type;
    struct DATA_TYPE_NODE *next;
} DATA_TYPE_NODE;

typedef struct moduleInfo
{
    // stores info related to module
    DATA_TYPE_NODE *input_plist;
    DATA_TYPE_NODE *output_plist;
    symbolTable *nested_table;
} moduleInfo;

typedef struct loopInfo
{
    LOOP_TYPE loop_type;
    symbolTable *nested_table;
    int leftBound;
    int rightBound;
    astNode *loopVar;  // null for while-loop
    astNode *loopExpr; // null for for-loop
} loopInfo;

typedef struct conditionalInfo
{
    PRIMITIVE_TYPE type; // arithmetic or boolean
    astNode *idNode;
    symbolTable *nested_table;
} conditionalInfo;

typedef struct caseInfo
{
    PRIMITIVE_TYPE type;
    int ifInt;
    bool ifBool;
    // astNode *caseNode;
    symbolTable *nested_table;
} caseInfo;

typedef struct variableInfo
{
    DATA_TYPE type;
    // semantic checking
    bool isAssigned;
    bool isLoopVar;
    int actualOffset;
    int printOffset;

} variableInfo;

typedef union STEntryInfo
{
    moduleInfo ifModule;
    variableInfo ifVariable;
    conditionalInfo ifConditional;
    loopInfo ifLoop;
    caseInfo ifCase;
} STEntryInfo;

struct symbolTableEntry
{
    char *name;
    int lineNo;
    STEntryInfo *info;
    symbolTableEntryType entryType;
};

struct symbolTable
{
    symbolTable *parent;
    symbolTableEntry *parentEntry;
    symbolTableEntry **table;
    SCOPE scope;
    int actualOffset;
    int printOffset;
};

typedef enum VARIABLE_TYPE
{
    LOCAL_VAR,
    INPUT_VAR,
    OUTPUT_VAR

} VARIABLE_TYPE;


typedef struct ArrayDetails
{
    char *moduleName;
    int start;
    int end;
    char *lexeme;
    DATA_TYPE dataType;
} ArrayDetails;




typedef enum ErrorTypes
{
    MODULE_REDEFINED,
    MODULE_NOT_DEFINED,
    MODULE_REDECLARED,
    MODULE_NOT_DECLARED,
    INVALID_NUMBER_OF_OUTPUT_PARAMETERS,
    INVALID_NUMBER_OF_INPUT_PARAMETERS,
    MODULE_OUTPUT_NOT_CAPTURED,
    INVALID_CONDITION,
    CASE_REDEFINED,
    VARIABLE_REDECLARED,
    VARIABLE_NOT_DECLARED,
    VARIABLE_IS_ARRAY,
    VARIABLE_IS_NOT_ARRAY,
    INVALID_VARIABLE_TYPE,
    INVALID_INPUT_PARAMETER,
    INVALID_WHILE_SEMANTICS,
    VARIABLE_IS_LOOP_COUNTER,
    ARRAY_INDEX_OUT_OF_BOUNDS,
    INVALID_ARRAY_BOUNDS,
    RETURN_VALUE_NOT_ASSIGNED,
    VARIABLE_NOT_ASSIGNED,
    RECURSIVE_CALL_NOT_ALLOWED,
    INVALID_ASSIGNMENT_TYPE_MISMATCH,
    INVALID_INDEXING,
    INVALID_OPERAND_TYPE,
    INVALID_CASE_TYPE,
    DEFAULT_NOT_FOUND

} ErrorType;





symbolTable *createSymbolTable(symbolTable *parent, symbolTableEntry *parentEntry);
symbolTableEntry *createSymbolTableEntry(char *name, int lineNo, STEntryInfo *info, symbolTableEntryType entryType);
void insertInSymbolTable(symbolTable *Table, symbolTableEntry *entry);
symbolTableEntry *lookUpInSymbolTable(symbolTable *Table, char *name);
symbolTableEntry *searchInTables(symbolTable *table, char *name, int lineNo);
void updateOffset(symbolTable *table, symbolTableEntry *entry, VARIABLE_TYPE var_type);
void printErrorScope(ErrorType errorType, int st, int en);
void printError(ErrorType errorType, char *name, int lineNo);
DATA_TYPE findTypeOfNode(astNode *node);
DATA_TYPE_NODE *copyIOList(astNode *module, symbolTable *moduleTable, bool isInput);
char *generateIdentifier(TOKEN token);
void PopulateSymbolTable(astNode *currNode, symbolTable *currTable, symbolTable *defTable);
void printIOlist(FILE *fp, DATA_TYPE_NODE *list);
void printSymbolTable(FILE *fp, symbolTable *table, char *constructName, int level);
void printST(symbolTable *table, char *constructName, int level);
bool compileTimeBoundCheck(DATA_TYPE arrDT, int index);
void getExpressionIds(astNode *exprNode, char **lexemeArr, int *index);
bool checkIfPresent(char **lexemeArr, int sz, char *lexeme);
bool checkWhileSemantics(astNode *stmtNode, char **lexemeArr, int sz, symbolTable* localTable);
void initArrayList();
void populateArrayList(char* moduleName, int start, int end, char *lexeme, DATA_TYPE dataType);
void printArrayList();
DATA_TYPE findExpressionType(astNode *exprNode, symbolTable *currTable);
DATA_TYPE findOperatorType(astNode *opNode, symbolTable *currTable);
DATA_TYPE findIDType(astNode *idNode, symbolTable *currTable);
void semanticCheck(astNode *currNode, symbolTable *currTable, symbolTable *globalTable, symbolTable *defTable);
void generateSymbolTable(char *input, char *output);
void printTillSemanticCheckTime(char *input, char *output);
int CountParseTreeNodes(ParseTreeNode *root);
int CountASTNodes(astNode *root);
void findActivationRecordSize(symbolTable* globalTable);
void MemoryCompressionDetails(char *input, char *output);
void printArrayDetails(char *input, char *output);
void printActivationRecordSize(char *input, char *output);
void MainDriver(char *input, char *output, int no);
char *findEnclosingModuleName(astNode *node);


# endif /* symboltable_h */