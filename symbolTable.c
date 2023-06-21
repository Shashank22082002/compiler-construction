
/*

//////////////////////////TEAM//////////////////////////
BITS ID             Member Name
2020A7PS0073P       Shashank Agrawal
2020A7PS0096P       Akshat Gupta
2020A7PS0129P       Anish Ghule
2020A7PS0134P       Shadan Hussain
2020A7PS1513P       Tarak P V S

*/
#include "symbolTable.h"
#include "parseTree.h"
#include "ast.h"
#include "global.h"
#include "time.h"
#include<string.h>

ArrayDetails arrayList[100];
int arrayCount = 0;
symbolTable *createSymbolTable(symbolTable *parent, symbolTableEntry *parentEntry)
{
    symbolTable *newSymbolTable = malloc(sizeof(symbolTable));
    newSymbolTable->parent = parent;
    newSymbolTable->parentEntry = parentEntry;
    if (parent == NULL)
    {
        newSymbolTable->actualOffset = 0;
        newSymbolTable->printOffset = 0;
    }
    else
    {
        newSymbolTable->actualOffset = parent->actualOffset;
        newSymbolTable->printOffset = parent->printOffset;
    }

    newSymbolTable->table = calloc(LIST_SIZE, sizeof(symbolTableEntry *));
    return newSymbolTable;
}

// store hash and not strings

symbolTableEntry *createSymbolTableEntry(char *name, int lineNo, STEntryInfo *info, symbolTableEntryType entryType)
{
    symbolTableEntry *newSymbolTableEntry = malloc(sizeof(symbolTableEntry));
    newSymbolTableEntry->name = malloc(strlen(name) + 1);
    strcpy(newSymbolTableEntry->name, name);
    newSymbolTableEntry->lineNo = lineNo;
    newSymbolTableEntry->info = info;
    newSymbolTableEntry->entryType = entryType;
    return newSymbolTableEntry;
}


void insertInSymbolTable(symbolTable *Table, symbolTableEntry *entry)
{
    int hashValue = generateHashValue(entry->name);
    int i = 0;
    while (Table->table[hashValue] && Table->table[hashValue]->name != NULL)
    {
        hashValue = quadraticProbing(hashValue, i);
        i++;
    }
    Table->table[hashValue] = entry;
}

symbolTableEntry *lookUpInSymbolTable(symbolTable *Table, char *name)
{
    int hashValue = generateHashValue(name);
    int i = 0;

    while (Table->table[hashValue] && Table->table[hashValue]->name != NULL)
    {
        if (strcmp(Table->table[hashValue]->name, name) == 0)
            return Table->table[hashValue];
        hashValue = quadraticProbing(hashValue, i);
        i++;
    }
    return NULL;
}

symbolTableEntry *searchInTables(symbolTable *table, char *name, int lineNo)
{
    if (table->parent == NULL)
        return NULL;
    symbolTableEntry *entry = lookUpInSymbolTable(table, name);
    if (entry != NULL && entry->lineNo <= lineNo)
        return entry;
    return searchInTables(table->parent, name, lineNo);
}


void updateOffset(symbolTable *table, symbolTableEntry *entry, VARIABLE_TYPE var_type)
{
    entry->info->ifVariable.actualOffset = table->actualOffset;
    entry->info->ifVariable.printOffset = table->printOffset;
    DATA_TYPE type = entry->info->ifVariable.type;
    // int width = type == INTEGER_TYPE ? _INTEGER_WIDTH_ : type == REAL_TYPE ? _REAL_WIDTH_
    //                                                                        : BOOLEAN_TYPE;
    // int extra = 1;
    // extra += entry->info->ifVariable.type.arrType == STATIC ? (entry->info->ifVariable.type.rightBound - entry->info->ifVariable.type.leftBound + 1) * width : 0;
    // table->offset += entry->info->ifVariable.type.arrType == NOT_ARRAY ? width : extra;

    int actual_width = 0;
    int print_width = 0;

    if (type.arrType == NOT_ARRAY)
    {
        if (type.primitiveType == INTEGER_TYPE)
        {
            actual_width = _ACT_INT_WIDTH_;
            print_width = _INTEGER_WIDTH_;
        }
        else if (type.primitiveType == REAL_TYPE)
        {
            actual_width = _ACT_REAL_WIDTH_;
            print_width = _REAL_WIDTH_;
        }
        else if (type.primitiveType == BOOLEAN_TYPE)
        {
            actual_width = _ACT_BOOLEAN_WIDTH_;
            print_width = _BOOLEAN_WIDTH_;
        }
    }
    else
    {
        switch (var_type)
        {
        case LOCAL_VAR:
        {
            if (type.primitiveType == INTEGER_TYPE)
            {
                actual_width = _ACT_INT_WIDTH_;
                print_width = _INTEGER_WIDTH_;
            }
            else if (type.primitiveType == REAL_TYPE)
            {
                actual_width = _ACT_REAL_WIDTH_;
                print_width = _REAL_WIDTH_;
            }
            else if (type.primitiveType == BOOLEAN_TYPE)
            {
                actual_width = _ACT_BOOLEAN_WIDTH_;
                print_width = _BOOLEAN_WIDTH_;
            }

            if (type.arrType == STATIC)
            {
                // space for the entire array
                print_width = 1 + (type.rightBound - type.leftBound + 1) * print_width;
                actual_width = _MIN_SP_SIZE_ + (type.rightBound - type.leftBound + 1) * actual_width;
            }
            else
                print_width = 1,                                // just for printing
            actual_width = _MIN_SP_SIZE_ + 2 * _ACT_INT_WIDTH_; // internally, we store the bounds as well

            break;
        }

        case INPUT_VAR:
        {
            print_width = 1 + 2 * _INTEGER_WIDTH_; // for base address and bounds
            actual_width = _MIN_SP_SIZE_ + 2 * _ACT_INT_WIDTH_;
            break;
        }
        }
    }

    // update the offset of all the tables until module table
    symbolTable *presentTable = table;

    while (presentTable->parent)
    {
        presentTable->printOffset += print_width;
        presentTable->actualOffset += actual_width;
        presentTable = presentTable->parent;
    }
}


void printErrorScope(ErrorType errorType, int st, int en)
{
    switch (errorType)
    {
    case INVALID_WHILE_SEMANTICS:
    {
        printf("\033[31mError: While Loop on lines %d - %d do not have an assignment for any expression ID\033[0m\n", st, en);
        break;
    }
    case DEFAULT_NOT_FOUND:
    {
        printf("\033[31mError: Switch - Case on lines %d - %d does not have a default case\033[0m\n", st, en);
    }
    }
}

void printError(ErrorType errorType, char *name, int lineNo)
{
    __SEMANTIC_ERROR__ = 1;
    switch (errorType)
    {
    case MODULE_REDEFINED:
    {
        printf("\033[31mError: Line %d: Module %s redefined\033[0m\n", lineNo, name);
        break;
    }
    case MODULE_NOT_DEFINED:
    {
        printf("\033[31mError: Line %d: Module %s not defined\033[0m\n", lineNo, name);
        break;
    }
    case MODULE_REDECLARED:
    {
        printf("\033[31mError: Line %d: Module %s redeclared\033[0m\n", lineNo, name);
        break;
    }
    case MODULE_NOT_DECLARED:
    {
        printf("\033[31mError: Line %d: Module %s not declared\033[0m\n", lineNo, name);
        break;
    }
    case INVALID_NUMBER_OF_OUTPUT_PARAMETERS:
    {
        printf("\033[31mError: Line %d: Module %s Invalid number of output parameters\033[0m\n", lineNo, name);
        break;
    }
    case INVALID_NUMBER_OF_INPUT_PARAMETERS:
    {
        printf("\033[31mError: Line %d: Module %s Invalid number of input parameters\033[0m\n", lineNo, name);
        break;
    }
    case MODULE_OUTPUT_NOT_CAPTURED:
    {
        printf("\033[31mError: Line %d: Module %s output not captured\033[0m\n", lineNo, name);
        break;
    }
    case CASE_REDEFINED:
    {
        printf("\033[31mError: Line %d: Case %s redefined\033[0m\n", lineNo, name);
        break;
    }
    case VARIABLE_REDECLARED:
    {
        printf("\033[31mError: Line %d: Variable %s redeclared\033[0m\n", lineNo, name);
        break;
    }

    case VARIABLE_NOT_DECLARED:
    {
        printf("\033[31mError: Line %d: Variable %s not declared\033[0m\n", lineNo, name);
        break;
    }
    case VARIABLE_IS_ARRAY:
    {
        printf("\033[31mError: Line %d: Variable %s is an array\033[0m\n", lineNo, name);
        break;
    }
    case VARIABLE_IS_NOT_ARRAY:
    {
        printf("\033[31mError: Line %d: Variable %s is not an array\033[0m\n", lineNo, name);
        break;
    }
    case VARIABLE_IS_LOOP_COUNTER:
    {
        printf("\033[31mError: Line %d: Variable %s is a loop counter\033[0m\n", lineNo, name);
        break;
    }
    case ARRAY_INDEX_OUT_OF_BOUNDS:
    {
        printf("\033[31mError: Line %d: Array index for Array %s out of bounds\033[0m\n", lineNo, name);
        break;
    }
    case INVALID_ARRAY_BOUNDS:
    {
        printf("\033[31mError: Line %d: Invalid array bounds for Array %s\033[0m\n", lineNo, name);
        break;
    }
    case INVALID_VARIABLE_TYPE:
    {
        printf("\033[31mError: Line %d: Type of Variable %s is INVALID\033[0m\n", lineNo, name);
        break;
    }
    case INVALID_INPUT_PARAMETER:
    {
        printf("\033[31mError: Line %d: Invalid input parameter for Module %s\033[0m\n", lineNo, name);
        break;
    }
    case RETURN_VALUE_NOT_ASSIGNED:
    {
        printf("\033[31mError: Line %d: Return variable %s not assigned\033[0m\n", lineNo, name);
        break;
    }
    case VARIABLE_NOT_ASSIGNED:
    {
        printf("\033[31mError: Line %d: Variable %s not assigned\033[0m\n", lineNo, name);
        break;
    }
    case RECURSIVE_CALL_NOT_ALLOWED:
    {
        printf("\033[31mError: Line %d: Recursive call to Module %s not allowed\033[0m\n", lineNo, name);
        break;
    }
    case INVALID_ASSIGNMENT_TYPE_MISMATCH:
    {
        printf("\033[31mError: Line %d: Type mismatch for %s in assignment\033[0m\n", lineNo, name);
        break;
    }
    case INVALID_INDEXING:
    {
        printf("\033[31mError: Line %d: Invalid indexing for %s\033[0m\n", lineNo, name);
        break;
    }
    case INVALID_OPERAND_TYPE:
    {
        printf("\033[31mError: Line %d: Invalid operands for operator %s\033[0m\n", lineNo, name);
        break;
    }
    case INVALID_CONDITION:
    {
        printf("\033[31mError: Line %d: Invalid condition for while loop \033[0m\n", lineNo);
        break;
    }

    case INVALID_CASE_TYPE:
    {
        printf("\033[31mError: Line %d: Invalid case type for case %s\033[0m\n", lineNo, name);
        break;
    }

    // case DEFAULT_NOT_FOUND:
    // {
    //     printf("\033[31mError: Line %d: Default case not found\033[0m\n", lineNo);
    //     break;
    // }
    }
}


DATA_TYPE findTypeOfNode(astNode *node)
{
    DATA_TYPE type;
    type.arrType = NOT_ARRAY;
    type.leftBound = INT_MIN;
    type.rightBound = INT_MIN;
    if (node->token->tokenType == INTEGER)
    {
        type.primitiveType = INTEGER_TYPE;
    }
    else if (node->token->tokenType == REAL)
    {
        type.primitiveType = REAL_TYPE;
    }
    else
    {
        type.primitiveType = BOOLEAN_TYPE;
    }

    if (node->children == NULL)
        return type;
    astNode *left = node->children->children;
    astNode *right = left->sibling;
    astNode *leftSign = left->children;
    astNode *rightSign = right->children;

    if (left->children->sibling->token->tokenType == ID)
    {
        type.leftID = malloc((strlen(leftSign->sibling->token->lexeme) + 1) * sizeof(char));
        strcpy(type.leftID, leftSign->sibling->token->lexeme);
        int sign = (leftSign->children == NULL) || (leftSign->children->token->tokenType == PLUS) ? 1 : -1;
        type.leftBound = sign;

        if (right->children->sibling->token->tokenType == ID)
        {
            type.arrType = DYNAMIC;
            type.rightID = malloc((strlen(rightSign->sibling->token->lexeme) + 1) * sizeof(char));
            strcpy(type.rightID, rightSign->sibling->token->lexeme);
            sign = (rightSign->children == NULL) || (rightSign->children->token->tokenType == PLUS) ? 1 : -1;
            type.rightBound = sign;
        }
        else
        {
            type.arrType = LEFT_DYNAMIC;
            int sign = (rightSign->children == NULL) || (rightSign->children->token->tokenType == PLUS) ? 1 : -1;
            type.rightBound = sign * right->children->sibling->token->tokenData.ifInt;
        }
    }
    else
    {
        if (right->children->sibling->token->tokenType == ID)
        {
            type.arrType = RIGHT_DYNAMIC;
            int sign = (leftSign->children == NULL) || (leftSign->children->token->tokenType == PLUS) ? 1 : -1;
            type.leftBound = sign * left->children->sibling->token->tokenData.ifInt;
            type.rightID = malloc((strlen(rightSign->sibling->token->lexeme) + 1) * sizeof(char));
            strcpy(type.rightID, rightSign->sibling->token->lexeme);
            sign = (rightSign->children == NULL) || (rightSign->children->token->tokenType == PLUS) ? 1 : -1;
            type.rightBound = sign;
        }
        else
        {
            type.arrType = STATIC;
            int sign = (leftSign->children == NULL) || (leftSign->children->token->tokenType == PLUS) ? 1 : -1;
            type.leftBound = sign * left->children->sibling->token->tokenData.ifInt;
            sign = (rightSign->children == NULL) || (rightSign->children->token->tokenType == PLUS) ? 1 : -1;
            type.rightBound = sign * right->children->sibling->token->tokenData.ifInt;
            if (type.leftBound > type.rightBound)
            {
                // printError(INVALID_ARRAY_BOUNDS, NULL, node->token->lineNo);
                type.primitiveType = INVALID_TYPE;
            }
        }
    }

    return type;
}


DATA_TYPE_NODE *copyIOList(astNode *module, symbolTable *moduleTable, bool isInput)
{
    astNode *child = isInput ? module->children->sibling->children : module->children->sibling->sibling->children;
    DATA_TYPE_NODE *head = NULL;
    DATA_TYPE_NODE *curr = NULL;
    while (child)
    {
        // first insert in module Table .. then in IOList in program Table
        symbolTableEntry *temp = lookUpInSymbolTable(moduleTable, child->token->lexeme);
        if (temp != NULL)
        {
            printError(VARIABLE_REDECLARED, child->token->lexeme, child->token->lineNo);
            child->ignore = true;
            module->ignore = true;
            child = child->sibling;
            continue;
        }
        DATA_TYPE varType = findTypeOfNode(child->children);
        if (!isInput && varType.arrType != NOT_ARRAY)
        {

            printError(VARIABLE_IS_ARRAY, child->token->lexeme, child->token->lineNo);
            child->ignore = true;
            module->ignore = true;
            child = child->sibling;
            continue;
        }
        if (varType.primitiveType == INVALID_TYPE)
        {
            printError(INVALID_ARRAY_BOUNDS, child->token->lexeme, child->token->lineNo);
            child->ignore = true;
            module->ignore = true;
            child = child->sibling;
            continue;
        }
        STEntryInfo *info = malloc(sizeof(STEntryInfo));
        info->ifVariable.type = varType;
        // todo value and offset
        info->ifVariable.isAssigned = isInput;
        info->ifVariable.isLoopVar = false;
        symbolTableEntry *newEntry = createSymbolTableEntry(child->token->lexeme, child->token->lineNo, info, VARIABLE);
        insertInSymbolTable(moduleTable, newEntry);

        DATA_TYPE_NODE *newNode = malloc(sizeof(DATA_TYPE_NODE));
        newNode->type = varType;
        newNode->next = NULL;

        if (head == NULL)
        {
            head = newNode;
            curr = newNode;
        }
        else
        {
            curr->next = newNode;
            curr = curr->next;
        }

        updateOffset(moduleTable, newEntry, isInput ? INPUT_VAR : OUTPUT_VAR);
        child = child->sibling;
    }
    return head;
}

char *generateIdentifier(TOKEN token)
{
    char *temp = malloc(sizeof(char) * 10);
    static int forcount = 1, whilecount = 1, switchcount = 1;

    if (token == FOR)
        sprintf(temp, "@for%d", forcount++);
    else if (token == WHILE)
        sprintf(temp, "@while%d", whilecount++);
    else if (token == SWITCH)
        sprintf(temp, "@switch%d", switchcount++);
    return temp;
}

void PopulateSymbolTable(astNode *currNode, symbolTable *currTable, symbolTable *defTable)
{
    // declarations of modules in currTable
    if (currNode == NULL)
        return;

    int varNo = currNode->variableNo;

    if (varNo == search(CharToNoMapping, "<program>"))
    {
        currTable->scope = currNode->scope;
        astNode *child = currNode->children;
        while (child)
        {
            PopulateSymbolTable(child, currTable, defTable);
            child = child->sibling;
        }
    }
    else if (varNo == search(CharToNoMapping, "<moduleDeclarations>"))
    {
        // create a symbol table entry for moduleDeclarations
        // check if there is any entry for otherModules since they must be at top
        astNode *child = currNode->children;

        while (child)
        {
            if (lookUpInSymbolTable(currTable, child->token->lexeme) != NULL)
            {
                printError(MODULE_REDECLARED, child->token->lexeme, child->token->lineNo);
                child->ignore = true;
                child = child->sibling;
                continue;
            }
            STEntryInfo *info = malloc(sizeof(STEntryInfo));
            // make symbol table entry for module declaration
            // no child symbol table created for declaration
            info->ifModule.nested_table = NULL;
            info->ifModule.input_plist = NULL;
            info->ifModule.output_plist = NULL;
            symbolTableEntry *newEntry = createSymbolTableEntry(child->token->lexeme, child->token->lineNo, info, OTHER_MODULE);
            insertInSymbolTable(currTable, newEntry);
            child = child->sibling;
        }
    }
    else if (varNo == search(CharToNoMapping, "<otherModules>"))
    {
        // create Symbol table entry as well as a symbol table linked to that entry
        astNode *child = currNode->children;
        while (child)
        {
            astNode *idNode = child->children;
            astNode *input_plist = idNode->sibling;
            astNode *output_plist = input_plist->sibling;
            astNode *module_def = output_plist->sibling;
            symbolTableEntry *temp = lookUpInSymbolTable(defTable, idNode->token->lexeme);

            if (temp != NULL && temp->info->ifModule.nested_table != NULL)
            {
                printError(MODULE_REDEFINED, idNode->token->lexeme, idNode->token->lineNo);
                child->ignore = true;
                child = child->sibling;
                continue;
            }
            else if (temp != NULL)
            {
                // create a new symbol table for the module
                temp->info->ifModule.nested_table = createSymbolTable(currTable, temp);
                temp->info->ifModule.input_plist = copyIOList(child, temp->info->ifModule.nested_table, 1);
                temp->info->ifModule.output_plist = copyIOList(child, temp->info->ifModule.nested_table, 0);
                // printf("Populating Symbol Table for Module %s\n", idNode->token->lexeme);
                temp->info->ifModule.nested_table->scope = child->scope;
                // PopulateSymbolTable(module_def, temp->info->ifModule.nested_table);
                // populate the symbol table
            }
            else
            {
                temp = createSymbolTableEntry(idNode->token->lexeme, idNode->token->lineNo, NULL, OTHER_MODULE);
                temp->info = malloc(sizeof(STEntryInfo));
                temp->info->ifModule.nested_table = createSymbolTable(currTable, temp);
                temp->info->ifModule.input_plist = copyIOList(child, temp->info->ifModule.nested_table, 1);
                temp->info->ifModule.output_plist = copyIOList(child, temp->info->ifModule.nested_table, 0);
                temp->info->ifModule.nested_table->scope = child->scope;
                insertInSymbolTable(defTable, temp);
                // printf("Populating Symbol Table for Module %s\n", idNode->token->lexeme);
                // PopulateSymbolTable(module_def, temp->info->ifModule.nested_table);
            }
            child = child->sibling;
        }
    }
    else if (varNo == DRIVER)
    {
        // create Symbol table entry as well as symbol table
        // need not check for presence of another drivermodule, handled at parser level
        // naming as driver
        symbolTableEntry *newEntry = createSymbolTableEntry("driver", currNode->token->lineNo, NULL, DRIVER_MODULE);
        newEntry->info = malloc(sizeof(STEntryInfo));
        newEntry->info->ifModule.input_plist = NULL;
        newEntry->info->ifModule.output_plist = NULL;
        newEntry->info->ifModule.nested_table = createSymbolTable(currTable, newEntry);
        newEntry->info->ifModule.nested_table->scope = currNode->scope;
        insertInSymbolTable(defTable, newEntry);
        // PopulateSymbolTable(currNode->children, newEntry->info->ifModule.nested_table);
    }

    else
    {
        // printf("Nothing to be done for %s\n", NoToCharMapping[varNo]);
    }
}

void printIOlist(FILE *fp, DATA_TYPE_NODE *list)
{
    if (list != NULL)
    {
        fprintf(fp, "%s", (list->type.primitiveType == INTEGER_TYPE) ? "INTEGER" : (list->type.primitiveType == REAL_TYPE) ? "REAL"
                                                                                                                           : "BOOLEAN");
        if (list->type.arrType != NOT_ARRAY)
            fprintf(fp, " ARRAY");
        list = list->next;
    }
    while (list != NULL)
    {
        fprintf(fp, ", %s", (list->type.primitiveType == INTEGER_TYPE) ? "INTEGER" : (list->type.primitiveType == REAL_TYPE) ? "REAL"
                                                                                                                             : "BOOLEAN");
        if (list->type.arrType != NOT_ARRAY)
            fprintf(fp, " ARRAY");
        list = list->next;
    }
}

void printSymbolTable(FILE *fp, symbolTable *table, char *constructName, int level)
{
    if (table == NULL)
    {
        printf("Error in printing symbol table\n");
        return;
    }

    char *notApplicable = "**";
    char lev[30];
    sprintf(lev, "%d", level);

    // printf("abc");
    // return;

    for (int i = 0; i < LIST_SIZE; i++)
    {
        symbolTableEntry *entry = table->table[i];
        if (entry == NULL)
            continue;

        // printf("abc");
        if (entry->entryType == VARIABLE)
        {
            // printf("def");
            char variableName[30];
            sprintf(variableName, "Variable: %s", entry->name);

            char scope[30];
            sprintf(scope, "[%d,%d]", entry->lineNo, table->scope.end);

            char *type = (entry->info->ifVariable.type.primitiveType == INTEGER_TYPE) ? "Integer" : (entry->info->ifVariable.type.primitiveType == REAL_TYPE) ? "Real"
                                                                                                                                                              : "Boolean";

            char *isArray = (entry->info->ifVariable.type.arrType == NOT_ARRAY) ? "No" : "Yes";

            char *stdy = (entry->info->ifVariable.type.arrType == STATIC) ? "Static" : (entry->info->ifVariable.type.arrType == LEFT_DYNAMIC) ? "Dynamic(L)"
                                                                                   : (entry->info->ifVariable.type.arrType == RIGHT_DYNAMIC)  ? "Dynamic(R)"
                                                                       : (entry->info->ifVariable.type.arrType == DYNAMIC)        ? "Dynamic(B)"
                                                                                                                                              : "**";

            int widt = (entry->info->ifVariable.type.primitiveType == INTEGER_TYPE) ? _INTEGER_WIDTH_ : (entry->info->ifVariable.type.primitiveType == REAL_TYPE) ? _REAL_WIDTH_
                                                                                                                                                                  : _BOOLEAN_WIDTH_;
            char width[30];

            int leftBound = entry->info->ifVariable.type.leftBound;
            int rightBound = entry->info->ifVariable.type.rightBound;

            char arrayRange[30];
            if (entry->info->ifVariable.type.arrType == NOT_ARRAY)
            {
                sprintf(arrayRange, "**");
                sprintf(width, "%d", widt);
            }
            else
            {
                if (entry->info->ifVariable.type.arrType == STATIC)
                {
                    sprintf(arrayRange, "[%d,%d]", leftBound, rightBound);
                    widt = abs(rightBound - leftBound + 1) * widt + 1;
                    sprintf(width, "%d", widt);
                }

                else if (entry->info->ifVariable.type.arrType == LEFT_DYNAMIC)
                {
                    sprintf(arrayRange, "[%c%s,%d]", (leftBound == -1 ? '-' : ' '), (entry->info->ifVariable.type.leftID), rightBound);
                    sprintf(width, "1");
                }
                else if (entry->info->ifVariable.type.arrType == RIGHT_DYNAMIC)
                {
                    sprintf(arrayRange, "[%d,%c%s]", leftBound, (rightBound == -1 ? '-' : ' '), (entry->info->ifVariable.type.rightID));
                    sprintf(width, "1");
                }
                else
                {
                    sprintf(arrayRange, "[%c%s,%c%s]", (leftBound == -1 ? '-' : ' '), (entry->info->ifVariable.type.leftID), (rightBound == -1 ? '-' : ' '), (entry->info->ifVariable.type.rightID));
                    sprintf(width, "1");
                }
            }

            char offset[30];
            sprintf(offset, "%d", entry->info->ifVariable.printOffset);
            // printf("%s",variableName);
            fprintf(fp, "%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s\n",
                    variableName, constructName, scope, type, isArray, stdy, arrayRange, width, offset, lev);
        }

        // else if (entry->entryType == OTHER_MODULE)
        // {
        //     // printf("aaa");
        //     char name[30];
        //     sprintf(name, "MODULE: %s", entry->name);

        //     char scope[30];
        //     // printf("[%d,%d]", entry->lineNo, (table->scope).end);
        //     sprintf(scope, "[%d,%d]", entry->lineNo, (table->scope).end);

        //     fprintf(fp, "%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s\n",
        //             name, constructName, scope, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, lev);

        //     fprintf(fp, "[Input Parameter List: ");
        //     printIOlist(fp, entry->info->ifModule.input_plist);
        //     fprintf(fp, "]\n");
        //     fprintf(fp, "[Output Parameter List: ");
        //     printIOlist(fp, entry->info->ifModule.output_plist);
        //     fprintf(fp, "]\n");
        // }
        // else if (entry->entryType == DRIVER_MODULE)
        // {
        //     // printf("ccc");
        //     char name[30];
        //     sprintf(name, "DRIVER: %s", entry->name);

        //     char scope[30];
        //     sprintf(scope, "[%d,%d]", entry->lineNo, table->scope.end);

        //     fprintf(fp, "%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s\n",
        //             name, constructName, scope, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, lev);
        // }
        // else if (entry->entryType == ITERATIVE_STMT && entry->info->ifLoop.loop_type == FOR_LOOP)
        // {
        //     char name[30];
        //     sprintf(name, "FOR: %s", entry->name);

        //     char scope[30];
        //     sprintf(scope, "[%d,%d]", entry->lineNo, table->scope.end);

        //     fprintf(fp, "%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s\n",
        //             name, constructName, scope, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, lev);
        // }
        // else if (entry->entryType == ITERATIVE_STMT && entry->info->ifLoop.loop_type == WHILE_LOOP)
        // {
        //     char name[30];
        //     sprintf(name, "WHILE: %s", entry->name);

        //     char scope[30];
        //     sprintf(scope, "[%d,%d]", entry->lineNo, table->scope.end);

        //     fprintf(fp, "%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s\n",
        //             name, constructName, scope, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, lev);
        // }
        // else if (entry->entryType == CONDITIONAL_STMT)
        // {
        //     char name[30];
        //     sprintf(name, "SWITCH: %s", entry->name);

        //     char scope[30];
        //     sprintf(scope, "[%d,%d]", entry->lineNo, table->scope.end);

        //     fprintf(fp, "%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s\n",
        //             name, constructName, scope, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, lev);
        // }
        // else if (entry->entryType == CASE_STMT)
        // {
        //     char name[30];
        //     sprintf(name, "CASE STMT: %s", entry->name);

        //     char scope[30];
        //     sprintf(scope, "[%d,%d]", entry->lineNo, table->scope.end);

        //     fprintf(fp, "%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s\n",
        //             name, constructName, scope, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, lev);
        // }
        else
        {
            fprintf(fp, "%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s\n",
                    notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable, notApplicable);

            // printf("Unrecognized entry\n");
        }
    }

    for (int i = 0; i < LIST_SIZE; i++)
    {
        symbolTableEntry *entry = table->table[i];
        if (entry == NULL)
            continue;
        else if (entry->entryType == OTHER_MODULE)
        {
            printSymbolTable(fp, entry->info->ifModule.nested_table, entry->name, level + 1);
        }
        else if (entry->entryType == DRIVER_MODULE)
        {
            printSymbolTable(fp, entry->info->ifModule.nested_table, entry->name, level + 1);
        }
        else if (entry->entryType == ITERATIVE_STMT && entry->info->ifLoop.loop_type == FOR_LOOP)
        {
            printSymbolTable(fp, entry->info->ifLoop.nested_table, entry->name, level + 1);
        }
        else if (entry->entryType == ITERATIVE_STMT && entry->info->ifLoop.loop_type == WHILE_LOOP)
        {
            printSymbolTable(fp, entry->info->ifLoop.nested_table, entry->name, level + 1);
        }
        else if (entry->entryType == CONDITIONAL_STMT)
        {
            printSymbolTable(fp, entry->info->ifConditional.nested_table, entry->name, level + 1);
        }
        else
            continue;
    }
}

void printST(symbolTable *table, char *constructName, int level)
{
    if (table == NULL)
    {
        printf("Error in printing symbol table\n");
        return;
    }

    printf("\n\nPrinting Symbol Table Started\n");

    FILE *fp = fopen("symbolTable.txt", "w");

    char *heading[10] = {" Entry Name", " Scope(Module Name)", " Scope(Line Numbers)", " Type Of Element", " Is Array", " Static/Dynamic(L/R/B)", " Array Range", " Width", " Offset", "Nesting Level"};
    fprintf(fp, "%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s|%-30s\n", heading[0], heading[1], heading[2], heading[3], heading[4], heading[5], heading[6], heading[7], heading[8], heading[9]);
    for (int i = 0; i < 300; i++)
        fprintf(fp, "_");
    fprintf(fp, "\n");

    printSymbolTable(fp, table, constructName, level);

    fclose(fp);
    printf("Printing Symbol Table Done\nCheck file symbolTable.txt\n\n");
}

bool compileTimeBoundCheck(DATA_TYPE arrDT, int index)
{
    // do compile time bound checking
    if (arrDT.arrType == STATIC)
    {
        if (index < arrDT.leftBound || index > arrDT.rightBound)
            return false;
    }
    else if (arrDT.arrType == LEFT_DYNAMIC)
    {
        if (index > arrDT.rightBound)
            return false;
    }
    else if (arrDT.arrType == RIGHT_DYNAMIC)
    {
        if (index < arrDT.leftBound)
            return false;
    }
    return true;
}

// given any ast expression node
// make a list of all ids

void getExpressionIds(astNode *exprNode, char **lexemeArr, int *index)
{
    if (exprNode == NULL)
        return;
    if (exprNode->variableNo == ID)
    {
        lexemeArr[*index] = malloc((strlen(exprNode->token->lexeme) + 1) * sizeof(char));
        strcpy(lexemeArr[*index], exprNode->token->lexeme);
        *index = *index + 1;
    }
    astNode *child = exprNode->children;
    while (child)
    {
        getExpressionIds(child, lexemeArr, index);
        child = child->sibling;
    }
}

bool checkIfPresent(char **lexemeArr, int sz, char *lexeme)
{
    for (int i = 0; i < sz; i++)
    {
        if (strcmp(lexemeArr[i], lexeme) == 0)
            return true;
    }
    return false;
}

bool checkWhileSemantics(astNode *stmtNode, char **lexemeArr, int sz, symbolTable* localTable)
{
    while (stmtNode)
    {
        if (stmtNode->variableNo == search(CharToNoMapping, "<assignmentStmt>"))
        {
            astNode *idNode = stmtNode->children;
            if (!lookUpInSymbolTable(localTable, idNode->token->lexeme) && checkIfPresent(lexemeArr, sz, idNode->token->lexeme))
                    return true;
        }
        stmtNode = stmtNode->sibling;
    }
    return false;
}

void initArrayList()
{
    arrayCount = 0;
    for (int i = 0; i < 100; i++)
    {
        arrayList[i].moduleName = NULL;
    }
}


void populateArrayList(char* moduleName, int start, int end, char *lexeme, DATA_TYPE dataType)
{
    arrayList[arrayCount].moduleName = moduleName;
    arrayList[arrayCount].start = start;
    arrayList[arrayCount].end = end;
    arrayList[arrayCount].lexeme = lexeme;
    arrayList[arrayCount].dataType = dataType;
    arrayCount++;
}

void printArrayList()
{
    printf("\n\nPrinting Array List :\n");
    for (int i = 0; i < 100; i++)
    {
        if (arrayList[i].moduleName == NULL)
            break;
        printf("%-20s", arrayList[i].moduleName);
        printf("%d-%d", arrayList[i].start, arrayList[i].end);
        printf("%-20s", " ");
        printf("%-20s", arrayList[i].lexeme);
        switch (arrayList[i].dataType.arrType)
        {
        case STATIC:
            printf("%-15s", "STATIC");
            printf("%d, %d", arrayList[i].dataType.leftBound, arrayList[i].dataType.rightBound);
            break;
        case LEFT_DYNAMIC:
            printf("%-15s", "LEFT_DYNAMIC");
            if(arrayList[i].dataType.leftBound == -1)
            {
                printf("-");
            }
            printf("%s, ", arrayList[i].dataType.leftID);
            printf("%d", arrayList[i].dataType.rightBound);
            break;
        case RIGHT_DYNAMIC:
            printf("%-15s", "RIGHT_DYNAMIC");
            printf("%d, ", arrayList[i].dataType.leftBound);
            if(arrayList[i].dataType.rightBound == -1)
            {
                printf("-");
            }
            printf("%s", arrayList[i].dataType.rightID);
            break;
        case DYNAMIC:
            printf("%-15s", "DYNAMIC");
            if(arrayList[i].dataType.leftBound == -1)
            {
                printf("-");
            }
            printf("%s, ", arrayList[i].dataType.leftID);
            if(arrayList[i].dataType.rightBound == -1)
            {
                printf("-");
            }
            printf("%s", arrayList[i].dataType.rightID);
            break;
        }
        printf("%-15s", " ");
        switch (arrayList[i].dataType.primitiveType)
        {
        case INTEGER_TYPE:
            printf("%-15s", "INTEGER");
            break;
        case REAL_TYPE:
            printf("%-15s", "REAL");
            break;
        case BOOLEAN_TYPE:
            printf("%-15s", "BOOLEAN");
            break;
        case INVALID_TYPE:
            printf("%-15s", "INVALID");
            break;
        }
        printf("\n");
    }
}

DATA_TYPE findOperatorType(astNode *opNode, symbolTable *currTable)
{
    DATA_TYPE dt;
    astNode *leftChild = opNode->children;
    astNode *rightChild = leftChild->sibling;
    DATA_TYPE leftType = findExpressionType(leftChild, currTable);
    DATA_TYPE rightType = findExpressionType(rightChild, currTable);
    if (leftType.primitiveType == INVALID_TYPE || rightType.primitiveType == INVALID_TYPE)
    {
        dt.primitiveType = INVALID_TYPE;
        dt.arrType = NOT_ARRAY;
        return dt;
    }
    if (leftType.arrType != NOT_ARRAY || rightType.arrType != NOT_ARRAY)
    {
        printError(INVALID_OPERAND_TYPE, opNode->token->lexeme, opNode->token->lineNo);
        dt.primitiveType = INVALID_TYPE;
        dt.arrType = NOT_ARRAY;
        return dt;
    }
    if (opNode->token->tokenType == PLUS || opNode->token->tokenType == MINUS || opNode->token->tokenType == MUL)
    {
        // types of left and right should be same
        // return type
        if (leftType.primitiveType == rightType.primitiveType && leftType.primitiveType != BOOLEAN_TYPE)
        {
            dt.primitiveType = leftType.primitiveType;
            dt.arrType = NOT_ARRAY;
            return dt;
        }
        else
        {
            printError(INVALID_OPERAND_TYPE, opNode->token->lexeme, opNode->token->lineNo);
            dt.primitiveType = INVALID_TYPE;
            dt.arrType = NOT_ARRAY;
            return dt;
        }
    }
    if (opNode->token->tokenType == DIV)
    {
        // should not be boolean
        if (leftType.primitiveType == BOOLEAN_TYPE || rightType.primitiveType == BOOLEAN_TYPE)
        {
            printError(INVALID_OPERAND_TYPE, opNode->token->lexeme, opNode->token->lineNo);
            dt.primitiveType = INVALID_TYPE;
            dt.arrType = NOT_ARRAY;
            return dt;
        }
        else
        {
            dt.primitiveType = REAL_TYPE;
            dt.arrType = NOT_ARRAY;
            return dt;
        }
    }
    if (opNode->token->tokenType == AND || opNode->token->tokenType == OR)
    {
        // should be boolean
        if (leftType.primitiveType == BOOLEAN_TYPE && rightType.primitiveType == BOOLEAN_TYPE)
        {
            dt.primitiveType = BOOLEAN_TYPE;
            dt.arrType = NOT_ARRAY;
            return dt;
        }
        else
        {
            printError(INVALID_OPERAND_TYPE, opNode->token->lexeme, opNode->token->lineNo);
            dt.primitiveType = INVALID_TYPE;
            dt.arrType = NOT_ARRAY;
            return dt;
        }
    }
    if (opNode->token->tokenType == LT || opNode->token->tokenType == LE || opNode->token->tokenType == GT || opNode->token->tokenType == GE || opNode->token->tokenType == EQ || opNode->token->tokenType == NE)
    {
        // should be same type
        if (leftType.primitiveType == rightType.primitiveType && leftType.primitiveType != BOOLEAN_TYPE)
        {
            dt.primitiveType = BOOLEAN_TYPE;
            dt.arrType = NOT_ARRAY;
            return dt;
        }
        else
        {
            printError(INVALID_OPERAND_TYPE, opNode->token->lexeme, opNode->token->lineNo);
            dt.primitiveType = INVALID_TYPE;
            dt.arrType = NOT_ARRAY;
            return dt;
        }
    }
}

DATA_TYPE findIDType(astNode *idNode, symbolTable *currTable)
{
    DATA_TYPE dt;
    symbolTableEntry *entry = searchInTables(currTable, idNode->token->lexeme, idNode->token->lineNo);
    if (entry == NULL || entry->entryType != VARIABLE)
    {
        printError(VARIABLE_NOT_DECLARED, idNode->token->lexeme, idNode->token->lineNo);
        dt.primitiveType = INVALID_TYPE;
        dt.arrType = NOT_ARRAY;
        return dt;
    }
    if (idNode->children == NULL)
    {
        return entry->info->ifVariable.type;
    }
    if (entry->info->ifVariable.type.arrType == NOT_ARRAY)
    {
        printError(INVALID_VARIABLE_TYPE, idNode->token->lexeme, idNode->token->lineNo);
        dt.primitiveType = INVALID_TYPE;
        dt.arrType = NOT_ARRAY;
        return dt;
    }
    // pakka array type he hai
    bool checkIndexBound = 1;

    if (idNode->children->variableNo == search(CharToNoMapping, "<element_index_with_expressions>"))
    {
        // int sign = (idNode->children->children->children->token->tokenType == MINUS) ? -1 : 1;
        int sign = idNode->children->children->children->token->tokenType == PLUS ? 1 : -1;
        if (idNode->children->children->sibling->variableNo == NUM)
            checkIndexBound = compileTimeBoundCheck(entry->info->ifVariable.type, sign * idNode->children->children->sibling->token->tokenData.ifInt);
         DATA_TYPE indexType = findExpressionType(idNode->children->children->sibling, currTable);

         if(indexType.primitiveType != INTEGER_TYPE || indexType.arrType != NOT_ARRAY)
         {
                printError(INVALID_INDEXING, idNode->token->lexeme, idNode->token->lineNo);
                dt.primitiveType = INVALID_TYPE;
                dt.arrType = NOT_ARRAY;
                return dt;
         }
    }
    else
    {
        if (idNode->children->variableNo == NUM)
            checkIndexBound = compileTimeBoundCheck(entry->info->ifVariable.type, idNode->children->token->tokenData.ifInt);
        DATA_TYPE indexType = findExpressionType(idNode->children, currTable);
        if (indexType.primitiveType != INTEGER_TYPE || indexType.arrType != NOT_ARRAY)
        {
            printError(INVALID_INDEXING, idNode->token->lexeme, idNode->token->lineNo);
            dt.primitiveType = INVALID_TYPE;
            dt.arrType = NOT_ARRAY;
            return dt;
        }
    }
    // check out of bounds for index
    if (!checkIndexBound)
    {
        // out of bounds
        printError(ARRAY_INDEX_OUT_OF_BOUNDS, idNode->token->lexeme, idNode->token->lineNo);
        dt.primitiveType = INVALID_TYPE;
        dt.arrType = NOT_ARRAY;
        return dt;
    }
    dt.arrType = NOT_ARRAY;
    dt.primitiveType = entry->info->ifVariable.type.primitiveType;
    return dt;
}

char *findEnclosingModuleName(astNode *node)
{
    if(node->variableNo == MODULE)
        return node->children->token->lexeme;
    else if (node->variableNo == DRIVER)
        return node->token->lexeme;
    return findEnclosingModuleName(node->parent);
}

DATA_TYPE findExpressionType(astNode *exprNode, symbolTable *currTable)
{
    DATA_TYPE dt;
    if (exprNode->variableNo == search(CharToNoMapping, "<U>"))
    {
        astNode *signNode = exprNode->children->children;
        DATA_TYPE expType = findExpressionType(exprNode->children->sibling, currTable);
        if (expType.primitiveType == INVALID_TYPE)
        {
            return expType;
        }
        if (!(expType.arrType == NOT_ARRAY) || (expType.primitiveType == BOOLEAN_TYPE))
        {
            printError(INVALID_OPERAND_TYPE, signNode->token->lexeme, signNode->token->lineNo);
            dt.primitiveType = INVALID_TYPE;
            dt.arrType = NOT_ARRAY;
            return dt;
        }
        return expType;
    }

    if (exprNode->variableNo == NUM)
    {
        dt.primitiveType = INTEGER_TYPE;
        dt.arrType = NOT_ARRAY;
    }
    else if (exprNode->variableNo == RNUM)
    {
        dt.primitiveType = REAL_TYPE;
        dt.arrType = NOT_ARRAY;
    }
    else if (exprNode->variableNo == TRUE || exprNode->token->tokenType == FALSE)
    {
        dt.primitiveType = BOOLEAN_TYPE;
        dt.arrType = NOT_ARRAY;
    }
    else if (exprNode->variableNo == ID)
    {
        dt = findIDType(exprNode, currTable);
    }
    // ab koi operator honge
    else
        dt = findOperatorType(exprNode, currTable);
    return dt;
}


void semanticCheck(astNode *currNode, symbolTable *currTable, symbolTable *globalTable, symbolTable *defTable)
{
    if (currNode->variableNo == search(CharToNoMapping, "<program>"))
    {
        // skip declarations -- no semanticCheck required
        astNode *child = currNode->children->sibling;
        while (child)
        {
            semanticCheck(child, currTable, globalTable, defTable);
            child = child->sibling;
        }
    }
    else if (currNode->variableNo == search(CharToNoMapping, "<otherModules>"))
    {
        // list of other modules
        astNode *child = currNode->children;
        while (child)
        {
            if (child->ignore)
            {
                child = child->sibling;
                continue;
            }
            semanticCheck(child, currTable, globalTable, defTable);
            child = child->sibling;
        }
    }
    else if (currNode->variableNo == DRIVER)
    {
        symbolTableEntry *driverEntry = lookUpInSymbolTable(defTable, "driver");
        insertInSymbolTable(currTable, driverEntry);
        semanticCheck(currNode->children, driverEntry->info->ifModule.nested_table, globalTable, defTable);
    }
    else if (currNode->variableNo == MODULE)
    {
        if (currNode->ignore)
            return;
        symbolTableEntry *moduleEntry = lookUpInSymbolTable(currTable, currNode->children->token->lexeme);
        symbolTableEntry *defEntry = lookUpInSymbolTable(defTable, currNode->children->token->lexeme);
        if (moduleEntry == NULL)
        {
            insertInSymbolTable(currTable, defEntry);
            moduleEntry = defEntry;
            defEntry->info->ifModule.nested_table->parent = currTable;
        }
        else
        {
            moduleEntry->info = defEntry->info;
            defEntry->info->ifModule.nested_table->parentEntry = moduleEntry;
            defEntry->info->ifModule.nested_table->parent = currTable;
        }
        semanticCheck(currNode->children->sibling->sibling->sibling, moduleEntry->info->ifModule.nested_table, globalTable, defTable);
        // check if all the values are assigned that are being returned
        astNode *retListNode = currNode->children->sibling->sibling->children;
        while(retListNode)
        {
            symbolTableEntry* idEntry = lookUpInSymbolTable(moduleEntry->info->ifModule.nested_table, retListNode->token->lexeme);
            if(idEntry->info->ifVariable.isAssigned == false)
            {
                printError(RETURN_VALUE_NOT_ASSIGNED, retListNode->token->lexeme, retListNode->token->lineNo);
                return;
            }
            retListNode = retListNode->sibling;
        }
    }
    else if (currNode->variableNo == search(CharToNoMapping, "<moduleDef>"))
    {
        // skip to statements
        astNode *stmtNode = currNode->children->children;
        while (stmtNode)
        {
            semanticCheck(stmtNode, currTable, globalTable, defTable);
            stmtNode = stmtNode->sibling;
        }
    }
    else if (currNode->variableNo == search(CharToNoMapping, "<statements>"))
    {
        // skip to statements
        astNode *stmtNode = currNode->children;
        while (stmtNode)
        {
            semanticCheck(stmtNode, currTable, globalTable, defTable);
            stmtNode = stmtNode->sibling;
        }
    }
    else if (currNode->variableNo == search(CharToNoMapping, "<declareStmt>"))
    {
        astNode *idListNode = currNode->children;
        astNode *dataTypeNode = idListNode->sibling;
        DATA_TYPE dataType = findTypeOfNode(dataTypeNode);
        if (dataType.primitiveType == INVALID_TYPE)
        {
            printError(INVALID_TYPE, dataTypeNode->token->lexeme, dataTypeNode->token->lineNo);
            currNode->ignore = true;
            return;
        }
        astNode *currId = idListNode->children;
        while (currId)
        {
            symbolTableEntry *temp = lookUpInSymbolTable(currTable, currId->token->lexeme);
            if (temp)
            {
                printError(VARIABLE_REDECLARED, currId->token->lexeme, currId->token->lineNo);
                currId->ignore = true;
                currId = currId->sibling;
                continue;
            }
    
            STEntryInfo *info = malloc(sizeof(STEntryInfo));
            info->ifVariable.type = dataType;
            info->ifVariable.isAssigned = false;
            info->ifVariable.isLoopVar = false;
            symbolTableEntry *newEntry = createSymbolTableEntry(currId->token->lexeme, currId->token->lineNo, info, VARIABLE);
            insertInSymbolTable(currTable, newEntry);
            updateOffset(currTable, newEntry, LOCAL_VAR);

            if(dataType.arrType != NOT_ARRAY)
            {
                populateArrayList(findEnclosingModuleName(currId), currId->token->lineNo, currTable->scope.end, currId->token->lexeme, dataType);
            }
            currId = currId->sibling;
            // todo variable offset and values
        }
    }
    else if (currNode->variableNo == search(CharToNoMapping, "<ioStmt>"))
    {
        astNode *ioType = currNode->children;
        if (ioType->token->tokenType == GET_VALUE)
        {
            astNode *idNode = ioType->sibling;
            symbolTableEntry *idEntry = searchInTables(currTable, idNode->token->lexeme, idNode->token->lineNo);
            if (idEntry == NULL || idEntry->entryType != VARIABLE)
            {
                printError(VARIABLE_NOT_DECLARED, idNode->token->lexeme, idNode->token->lineNo);
                return;
            }
            // if (idEntry->info->ifVariable.type.arrType != NOT_ARRAY)
            // {
            //     printError(VARIABLE_IS_ARRAY, idNode->token->lexeme, idNode->token->lineNo);
            //     return;
            // }
            if (idEntry->info->ifVariable.isLoopVar == true)
            {
                printError(VARIABLE_IS_LOOP_COUNTER, idNode->token->lexeme, idNode->token->lineNo);
                return;
            }
            idEntry->info->ifVariable.isAssigned = 1;
        }
        else
        {
            astNode *idNode = ioType->sibling;
            if (idNode->token->tokenType == NUM || idNode->token->tokenType == RNUM || idNode->token->tokenType == TRUE || idNode->token->tokenType == FALSE)
                return; // JUST PRINTING A CONSTANT
            symbolTableEntry *idEntry = searchInTables(currTable, idNode->token->lexeme, idNode->token->lineNo);
            if (idEntry == NULL || idEntry->entryType != VARIABLE)
            {
                printError(VARIABLE_NOT_DECLARED, idNode->token->lexeme, idNode->token->lineNo);
                return;
            }
            if (idNode->sibling == NULL)
            {
                // print value of variable
                return;
            }
            astNode *signNode = idNode->children;
            astNode *indexNode = signNode->sibling;
            if (indexNode->token->tokenType == NUM)
            {
                // do bound checking
                int val = indexNode->token->tokenData.ifInt * (signNode->children == NULL || signNode->children->token->tokenType == PLUS ? 1 : -1);
                bool res = compileTimeBoundCheck(idEntry->info->ifVariable.type, val);
                if (!res)
                {
                    printError(ARRAY_INDEX_OUT_OF_BOUNDS, indexNode->token->lexeme, indexNode->token->lineNo);
                    return;
                }
                // res true to print kardo
                return;
            }
            // ab ye id hai

            idEntry = searchInTables(currTable, indexNode->token->lexeme, indexNode->token->lineNo);
            if (idEntry == NULL || idEntry->entryType != VARIABLE)
            {
                printError(VARIABLE_NOT_DECLARED, indexNode->token->lexeme, indexNode->token->lineNo);
                return;
            }
            if (idEntry->info->ifVariable.type.arrType != NOT_ARRAY)
            {
                printError(VARIABLE_IS_ARRAY, indexNode->token->lexeme, indexNode->token->lineNo);
                return;
            }
            if (idEntry->info->ifVariable.type.primitiveType != INTEGER_TYPE)
            {
                printError(INVALID_VARIABLE_TYPE, indexNode->token->lexeme, indexNode->token->lineNo);
                return;
            }
            if (idEntry->info->ifVariable.isAssigned == 0)
            {
                printError(VARIABLE_NOT_ASSIGNED, indexNode->token->lexeme, indexNode->token->lineNo);
                return;
            }

            // ab sab sahi h
            // print kardo
        }
    }
    else if (currNode->variableNo == search(CharToNoMapping, "<assignmentStmt>"))
    {
        astNode *idNode = currNode->children;
        if (idNode->token->tokenType != ID)
        {
            printError(INVALID_VARIABLE_TYPE, idNode->token->lexeme, idNode->token->lineNo);
            return;
        }
        symbolTableEntry *idEntry = searchInTables(currTable, idNode->token->lexeme, idNode->token->lineNo);
        if (idEntry == NULL || idEntry->entryType != VARIABLE)
        {
            printError(VARIABLE_NOT_DECLARED, idNode->token->lexeme, idNode->token->lineNo);
            return;
        }
        if (idEntry->info->ifVariable.isLoopVar == true)
        {
            printError(VARIABLE_IS_LOOP_COUNTER, idNode->token->lexeme, idNode->token->lineNo);
            return;
        }
        // ab ye correct id hai
        DATA_TYPE idType = idEntry->info->ifVariable.type;
        astNode *lValType = idNode->sibling;
        if (lValType->variableNo == search(CharToNoMapping, "<lvalueIDStmt>"))
        {
            DATA_TYPE exprType = findExpressionType(lValType->children, currTable);

            if (idType.arrType == NOT_ARRAY)
            {
                if (exprType.primitiveType != idType.primitiveType || exprType.primitiveType == INVALID_TYPE)
                {
                    if (exprType.primitiveType != INVALID_TYPE)
                        printError(INVALID_VARIABLE_TYPE, idNode->token->lexeme, idNode->token->lineNo);
                    return;
                }
                if (exprType.arrType != NOT_ARRAY)
                {
                    printError(INVALID_VARIABLE_TYPE, idNode->token->lexeme, idNode->token->lineNo);
                    return;
                }
                // ab ye sahi h
                // code generation
            }
            else
            {
                // Check if RHS is not an array
                if(exprType.arrType == NOT_ARRAY)
                {
                    printError(INVALID_VARIABLE_TYPE, idNode->token->lexeme, idNode->token->lineNo);
                    return;
                }

                // Check types for array
                // OLD LOGIC FOR CHECKING TYPES
                // if (exprType.arrType == STATIC || exprType.arrType == LEFT_DYNAMIC)
                // {
                //     if (idType.arrType == STATIC || idType.arrType == LEFT_DYNAMIC)
                //     {
                //         if (exprType.rightBound != idType.rightBound)
                //         {
                //             printError(INVALID_ASSIGNMENT_TYPE_MISMATCH, idNode->token->lexeme, idNode->token->lineNo);
                //             return;
                //         }
                //     }
                // }
                // if (exprType.arrType == STATIC || exprType.arrType == RIGHT_DYNAMIC)
                // {
                //     if (idType.arrType == STATIC || idType.arrType == RIGHT_DYNAMIC)
                //     {
                //         if (exprType.leftBound != idType.leftBound)
                //         {
                //             printError(INVALID_ASSIGNMENT_TYPE_MISMATCH, idNode->token->lexeme, idNode->token->lineNo);
                //             return;
                //         }
                //     }
                // }
                if (exprType.arrType == STATIC && idType.arrType == STATIC)
                {
                    if ((exprType.rightBound - exprType.leftBound) != (idType.rightBound - idType.leftBound))
                    {
                        printError(INVALID_ASSIGNMENT_TYPE_MISMATCH, idNode->token->lexeme, idNode->token->lineNo);
                        return;
                    }
                }
                if(exprType.primitiveType != idType.primitiveType)
                {
                    printError(INVALID_ASSIGNMENT_TYPE_MISMATCH, idNode->token->lexeme, idNode->token->lineNo);
                    return;
                }
                // ab ye sahi h 
            }
            idEntry->info->ifVariable.isAssigned = 1;
        }
        else
        {
            // yahan lvalArrStatement h
            DATA_TYPE indexType;
            if (idEntry->info->ifVariable.type.arrType == NOT_ARRAY)
            {
                printError(VARIABLE_IS_NOT_ARRAY, idNode->token->lexeme, idNode->token->lineNo);
                return;
            }
            int index = INT_MIN;
            // TODO BOUND CHECKING
            if (lValType->children->variableNo == search(CharToNoMapping, "<element_index_with_expressions>"))
            {
                if (lValType->children->sibling->variableNo == NUM)
                    index = lValType->children->sibling->token->tokenData.ifInt * (lValType->children->children->children == NULL || lValType->children->children->children->token->tokenType == PLUS ? 1 : -1);
                indexType = findExpressionType(lValType->children->children->sibling, currTable);
            }
            else
            {
                if (lValType->children->variableNo == NUM)
                    index = lValType->children->token->tokenData.ifInt;
                indexType = findExpressionType(lValType->children, currTable);
            }
            if (indexType.primitiveType != INTEGER_TYPE || indexType.arrType != NOT_ARRAY || indexType.primitiveType == INVALID_TYPE)
            {
                if (indexType.primitiveType != INVALID_TYPE)
                    printError(INVALID_INDEXING, idNode->token->lexeme, idNode->token->lineNo);
                return;
            }
            if (index != INT_MIN)
            {
                bool res = compileTimeBoundCheck(idEntry->info->ifVariable.type, index);
                if (!res)
                {
                    printError(ARRAY_INDEX_OUT_OF_BOUNDS, idNode->token->lexeme, idNode->token->lineNo);
                    return;
                }
            }
            // ab index sahi h
            DATA_TYPE exprType = findExpressionType(lValType->children->sibling, currTable);
            if (exprType.arrType != NOT_ARRAY || exprType.primitiveType != idType.primitiveType || exprType.primitiveType == INVALID_TYPE)
            {
                if (exprType.primitiveType != INVALID_TYPE)
                    printError(INVALID_VARIABLE_TYPE, idNode->token->lexeme, idNode->token->lineNo);
                return;
            }
        }
    }
    else if (currNode->variableNo == search(CharToNoMapping, "<moduleReuseStmt>"))
    {
        // <optional> ID <actual_para_list>
        astNode *opList = currNode->children;
        astNode *moduleId = opList->sibling;
        astNode *actParaList = moduleId->sibling;

        symbolTableEntry *idEntry = lookUpInSymbolTable(globalTable, moduleId->token->lexeme);

        if (idEntry == NULL)
        {
            printError(MODULE_NOT_DECLARED, moduleId->token->lexeme, moduleId->token->lineNo);
            return;
        }

        if (idEntry->info->ifModule.nested_table == NULL)
        {
            symbolTableEntry *defEntry = lookUpInSymbolTable(defTable, moduleId->token->lexeme);
            if (defEntry)
            {
                idEntry = defEntry;
            }
            else
            {
                printError(MODULE_NOT_DEFINED, moduleId->token->lexeme, moduleId->token->lineNo);
                return;
            }
        }
        // ab module to defined h
        // check karo ki output parameters sahi h
        //    check types is matching
        //    check all variables should be declared till now
        DATA_TYPE_NODE *retListNode = idEntry->info->ifModule.output_plist;
        astNode *idList = opList->children;
        if (idList == NULL && retListNode != NULL)
        {
            printError(MODULE_OUTPUT_NOT_CAPTURED, moduleId->token->lexeme, moduleId->token->lineNo);
            return;
        }
        else if(idList == NULL && retListNode == NULL)
        {
            
        }
        else
        {
            // ab koi null nahi h
            astNode *idListNode = idList->children;
            while (idListNode != NULL && retListNode != NULL)
            {
                symbolTableEntry *idEntry = searchInTables(currTable, idListNode->token->lexeme, idListNode->token->lineNo);
                if (idEntry == NULL)
                {
                    printError(VARIABLE_NOT_DECLARED, idListNode->token->lexeme, idListNode->token->lineNo);
                    return;
                }
                if (idEntry->info->ifVariable.type.arrType != NOT_ARRAY)
                {
                    printError(VARIABLE_IS_ARRAY, idListNode->token->lexeme, idListNode->token->lineNo);
                    return;
                }
                if (idEntry->info->ifVariable.isLoopVar)
                {
                    printError(VARIABLE_IS_LOOP_COUNTER, idListNode->token->lexeme, idListNode->token->lineNo);
                    return;
                }
                if (idEntry->info->ifVariable.type.primitiveType != retListNode->type.primitiveType)
                {
                    printError(INVALID_VARIABLE_TYPE, idListNode->token->lexeme, idListNode->token->lineNo);
                    return;
                }
                idListNode = idListNode->sibling;
                retListNode = retListNode->next;
            }
            if (idListNode != NULL || retListNode != NULL)
            {
                printError(INVALID_NUMBER_OF_OUTPUT_PARAMETERS, moduleId->token->lexeme, moduleId->token->lineNo);
                return;
            }
        }
        // ab output parameters sahi h
        // check karo ki input parameters sahi h
        //    check types is matching
        //    check all variables should be declared till now and not out of bounds
        DATA_TYPE_NODE *inputListNode = idEntry->info->ifModule.input_plist;
        astNode *actParaListNode = actParaList->children;
        while (inputListNode != NULL && actParaListNode != NULL)
        {
            DATA_TYPE exprType;
            if (actParaListNode->children->variableNo == TRUE || actParaListNode->children->variableNo == FALSE)
            {
                exprType.primitiveType = BOOLEAN_TYPE;
                exprType.arrType = NOT_ARRAY;
            }
            else
            {
                // abhi boolConstt nhi h
                exprType = findExpressionType(actParaListNode->children->sibling, currTable);
                if (exprType.primitiveType == INVALID_TYPE)
                    return;
                if ((exprType.arrType != NOT_ARRAY) && actParaListNode->children->children)
                {
                    printError(INVALID_INPUT_PARAMETER, moduleId->token->lexeme, moduleId->token->lineNo);
                    return;
                }

                if (exprType.primitiveType != inputListNode->type.primitiveType)
                {
                    printError(INVALID_INPUT_PARAMETER, moduleId->token->lexeme, moduleId->token->lineNo);
                    return;
                }
                if ((exprType.arrType != NOT_ARRAY && inputListNode->type.arrType == NOT_ARRAY) || (exprType.arrType == NOT_ARRAY && inputListNode->type.arrType != NOT_ARRAY))
                {
                    printError(INVALID_INPUT_PARAMETER, moduleId->token->lexeme, moduleId->token->lineNo);
                    return;
                }
                if ((exprType.arrType == STATIC || exprType.arrType == LEFT_DYNAMIC) && (inputListNode->type.arrType == STATIC || inputListNode->type.arrType == LEFT_DYNAMIC))
                {
                    if (exprType.rightBound != inputListNode->type.rightBound)
                    {
                        printError(INVALID_INPUT_PARAMETER, moduleId->token->lexeme, moduleId->token->lineNo);
                        return;
                    }
                }
                if ((exprType.arrType == STATIC || exprType.arrType == RIGHT_DYNAMIC) && (inputListNode->type.arrType == STATIC || inputListNode->type.arrType == RIGHT_DYNAMIC))
                {
                    if (exprType.leftBound != inputListNode->type.leftBound)
                    {
                        printError(INVALID_INPUT_PARAMETER, moduleId->token->lexeme, moduleId->token->lineNo);
                        return;
                    }
                }
                // ab sab sahi h
            }
            inputListNode = inputListNode->next;
            actParaListNode = actParaListNode->sibling;
        }
        if (inputListNode != NULL || actParaListNode != NULL)
        {
            printError(INVALID_NUMBER_OF_INPUT_PARAMETERS, moduleId->token->lexeme, moduleId->token->lineNo);
            return;
        }
        symbolTable* parentTable = currTable->parent;
        symbolTableEntry *parentEntry = currTable->parentEntry;
        while(parentTable->parent != NULL)
        {
            parentEntry = parentTable->parentEntry;
            parentTable = parentTable->parent;
        }
        if(strcmp(parentEntry->name, moduleId->token->lexeme) == 0)
        {
            printError(RECURSIVE_CALL_NOT_ALLOWED, moduleId->token->lexeme, moduleId->token->lineNo);
            return;
        }
    }
    else if (currNode->variableNo == FOR)
    {
        // entry + table
        char *hashFor = generateIdentifier(FOR);
        strcpy(currNode->token->lexeme, hashFor);
        astNode *idNode = currNode->children;
        astNode *rangeNode = idNode->sibling;
        astNode *leftRange = rangeNode->children;
        astNode *rightRange = leftRange->sibling;
        astNode *statementsNode = rangeNode->sibling;
        int leftSign = (leftRange->children->token->tokenType == NUM || leftRange->children->token->tokenType == PLUS) ? 1 : -1;
        int rightSign = (rightRange->children->token->tokenType == NUM || rightRange->children->token->tokenType == PLUS) ? 1 : -1;
        int leftValue = (leftRange->children->token->tokenType == NUM) ? leftRange->children->token->tokenData.ifInt : leftRange->children->sibling->token->tokenData.ifInt;
        int rightValue = (rightRange->children->token->tokenType == NUM) ? rightRange->children->token->tokenData.ifInt : rightRange->children->sibling->token->tokenData.ifInt;
        STEntryInfo *info = malloc(sizeof(STEntryInfo));
        info->ifLoop.leftBound = leftSign * leftValue;
        info->ifLoop.rightBound = rightSign * rightValue;
        info->ifLoop.loopExpr = NULL;
        info->ifLoop.loopVar = idNode;
        info->ifLoop.loop_type = FOR_LOOP;
        symbolTableEntry *newEntry = createSymbolTableEntry(hashFor, currNode->token->lineNo, info, ITERATIVE_STMT);
        insertInSymbolTable(currTable, newEntry);
        info->ifLoop.nested_table = createSymbolTable(currTable, newEntry);
        info->ifLoop.nested_table->scope = currNode->scope;
        STEntryInfo *idInfo = malloc(sizeof(STEntryInfo));
        idInfo->ifVariable.type.primitiveType = INTEGER_TYPE;
        idInfo->ifVariable.type.arrType = NOT_ARRAY;
        idInfo->ifVariable.type.leftBound = INT_MIN;
        idInfo->ifVariable.type.rightBound = INT_MIN;
        idInfo->ifVariable.isLoopVar = true;
        idInfo->ifVariable.isAssigned = true;
        symbolTableEntry *idEntry = createSymbolTableEntry(idNode->token->lexeme, idNode->token->lineNo, idInfo, VARIABLE);
        insertInSymbolTable(info->ifLoop.nested_table, idEntry);
        updateOffset(info->ifLoop.nested_table, idEntry, LOCAL_VAR);
        semanticCheck(currNode->children->sibling->sibling, info->ifLoop.nested_table, globalTable, defTable);
    }
    else if (currNode->variableNo == WHILE)
    {
        astNode *exprNode = currNode->children;
        astNode *statementsNode = exprNode->sibling;
        char *hashWhile = generateIdentifier(WHILE);
        strcpy(currNode->token->lexeme, hashWhile);
        STEntryInfo *info = malloc(sizeof(STEntryInfo));
        info->ifLoop.leftBound = INT_MIN;
        info->ifLoop.rightBound = INT_MIN;
        info->ifLoop.loopExpr = exprNode;
        info->ifLoop.loopVar = NULL;
        info->ifLoop.loop_type = WHILE_LOOP;
        symbolTableEntry *idEntry = createSymbolTableEntry(hashWhile, currNode->token->lineNo, info, ITERATIVE_STMT);
        insertInSymbolTable(currTable, idEntry);
        info->ifLoop.nested_table = createSymbolTable(currTable, idEntry);
        info->ifLoop.nested_table->scope = currNode->scope;
        // symbolTableEntry *idEntry = searchInTables(currTable, currNode->token->lexeme, currNode->token->lineNo);
        DATA_TYPE dt = findExpressionType(currNode->children, currTable);
        // todo
        // find list of all ids in findExpressionType
        // Check if any of them gets assigned in the loop
        if (dt.arrType != NOT_ARRAY || dt.primitiveType != BOOLEAN_TYPE)
        {
            printError(INVALID_CONDITION, currNode->token->lexeme, currNode->token->lineNo);
            return;
        }
        semanticCheck(currNode->children->sibling, idEntry->info->ifLoop.nested_table, globalTable, defTable);
        // function 1 == returns an array of all id lexemes
        // function 2 == iterates on statements and check if lvalue is present in the list
        // function 3 == check if a value is present in the list
        char *lexemeArr[50];
        int sz = 0;
        int *index = &sz;
        getExpressionIds(exprNode, lexemeArr, index);
        bool checkWhile = true;
        if (sz == 0)
            checkWhile = true;
        else
            checkWhile = checkWhileSemantics(statementsNode->children, lexemeArr, sz, info->ifLoop.nested_table);
        if (checkWhile == false)
        {
            printErrorScope(INVALID_WHILE_SEMANTICS, currNode->scope.start, currNode->scope.end);
            currNode->ignore = true;
            return;
        }
    }
    else if (currNode->variableNo == SWITCH)
    {
        astNode *idNode = currNode->children;
        symbolTableEntry *idEntry = searchInTables(currTable, idNode->token->lexeme, idNode->token->lineNo);
        if (!idEntry || idEntry->entryType != VARIABLE)
        {
            printError(VARIABLE_NOT_DECLARED, idNode->token->lexeme, idNode->token->lineNo);
            currNode->ignore = true;
            return;
        }

        if (idEntry->info->ifVariable.type.arrType != NOT_ARRAY || idEntry->info->ifVariable.type.primitiveType == REAL_TYPE)
        {
            printError(INVALID_VARIABLE_TYPE, idNode->token->lexeme, idNode->token->lineNo);
            currNode->ignore = true;
            return;
        }
        DATA_TYPE idType = idEntry->info->ifVariable.type;

        // now variable type is either boolean or integer

        astNode *caseListNode = idNode->sibling;
        astNode *defaultNode = caseListNode->sibling;
        char *hashSwitch = generateIdentifier(SWITCH);
        strcpy(currNode->token->lexeme, hashSwitch);
        STEntryInfo *info = malloc(sizeof(STEntryInfo));
        symbolTableEntry *newEntry = createSymbolTableEntry(hashSwitch, currNode->token->lineNo, info, CONDITIONAL_STMT);
        insertInSymbolTable(currTable, newEntry);
        info->ifConditional.nested_table = createSymbolTable(currTable, newEntry);
        info->ifConditional.nested_table->scope = currNode->scope;
        info->ifConditional.idNode = idNode;
        caseListNode = caseListNode->children;
        while (caseListNode)
        {
            if (idType.primitiveType == BOOLEAN_TYPE)
            {
                if (caseListNode->token->tokenType != TRUE && caseListNode->token->tokenType != FALSE)
                {
                    printError(INVALID_CASE_TYPE, caseListNode->token->lexeme, caseListNode->token->lineNo);
                    caseListNode->ignore = true;
                    caseListNode = caseListNode->sibling;
                    continue;
                }
            }
            else
            {
                if (caseListNode->token->tokenType != NUM)
                {
                    printError(INVALID_CASE_TYPE, caseListNode->token->lexeme, caseListNode->token->lineNo);
                    caseListNode->ignore = true;
                    caseListNode = caseListNode->sibling;
                    continue;
                }
            }

            symbolTableEntry *temp = lookUpInSymbolTable(info->ifConditional.nested_table, caseListNode->token->lexeme);
            if (temp)
            {
                printError(CASE_REDEFINED, caseListNode->token->lexeme, caseListNode->token->lineNo);
                caseListNode->ignore = true;
                caseListNode = caseListNode->sibling;
                continue;
            }
            STEntryInfo *caseInfo = malloc(sizeof(STEntryInfo));
            caseInfo->ifCase.type = caseListNode->token->tokenType == NUM ? INTEGER_TYPE : BOOLEAN_TYPE;
            symbolTableEntry *newEntry1 = createSymbolTableEntry(caseListNode->token->lexeme, caseListNode->token->lineNo, caseInfo, CASE_STMT);
            insertInSymbolTable(info->ifConditional.nested_table, newEntry1);
            // caseInfo->ifCase.caseNode = caseListNode->children;
            if (caseInfo->ifCase.type == INTEGER_TYPE)
                caseInfo->ifCase.ifInt = caseListNode->token->tokenData.ifInt;
            else
                caseInfo->ifCase.ifBool = caseListNode->token->tokenData.ifBool;

            caseInfo->ifCase.nested_table = createSymbolTable(info->ifConditional.nested_table, newEntry);
            insertInSymbolTable(caseInfo->ifCase.nested_table, newEntry1);

            // PopulateSymbolTable(caseListNode->children, caseInfo->ifCase.nested_table);
            semanticCheck(caseListNode->children, caseInfo->ifCase.nested_table, globalTable, defTable);
            caseListNode = caseListNode->sibling;
        }
        if (defaultNode)
        {
            if (idType.primitiveType == BOOLEAN_TYPE)
            {
                printError(INVALID_CASE_TYPE, defaultNode->token->lexeme, defaultNode->token->lineNo);
                defaultNode->ignore = true;
                return;
            }
            STEntryInfo *caseInfo = malloc(sizeof(STEntryInfo));
            symbolTableEntry *newEntry2 = createSymbolTableEntry(defaultNode->token->lexeme, defaultNode->token->lineNo, caseInfo, CASE_STMT);
            // insertInSymbolTable(info->ifConditional.nested_table, newEntry2);
            caseInfo->ifCase.nested_table = createSymbolTable(info->ifConditional.nested_table, newEntry);
            insertInSymbolTable(caseInfo->ifConditional.nested_table, newEntry2);
            // caseInfo->ifCase.caseNode = defaultNode->children;
            semanticCheck(defaultNode->children, caseInfo->ifCase.nested_table, globalTable, defTable);
        }
        else
        {
            if (idType.primitiveType == INTEGER_TYPE)
            {
                printErrorScope(DEFAULT_NOT_FOUND, info->ifConditional.nested_table->scope.start, info->ifConditional.nested_table->scope.end);
                currNode->ignore = true;
                return;
            }
        }
        // todo type of info->ifConditional.idNode
    }
}

void generateSymbolTable(char *input, char *output)
{
    // initialize lookup table
    lookupTable = initializeLookupTable();
    FILE *fp = fopen(input, "r");

    if (fp == NULL)
    {
        printf("Error opening source code file.");
        return;
    }

    // creating and populating buffers
    // freeMappings();
    buff1 = malloc(BUFF_SIZE * sizeof(char));
    buff2 = malloc(BUFF_SIZE * sizeof(char));
    populateBuffer(fp, buff1);
    if (!readCodeCompletely)
        populateBuffer(fp, buff2);
    ParseTree *parseTree = readGrammar(output, fp);
    if (__SYNTAX_ERROR__)
    {
        printf("\033[31mSyntax Errors Found.. Symbol Table not generated\033[0m");
        return;
    }
    AST *ast = createAST(parseTree);
    symbolTable *globalTable = createSymbolTable(NULL, NULL);
    symbolTable *defTable = createSymbolTable(NULL, NULL);
    printf("Checking Semantic Errors...\n");
    PopulateSymbolTable(ast->root, globalTable, defTable);
    semanticCheck(ast->root, globalTable, globalTable, defTable);
    // printArrayList();
    // printST(globalTable, "global", 0);
    if(__SEMANTIC_ERROR__)
        printf("\033[31mSemantic Errors Found...\033[0m\n");
    else
        printf("\033[32mCode Compiles Successfully...\033[0m\n");
    
    fseek(fp, 0, SEEK_SET);
    fclose(fp);
    free(buff1);
    free(buff2);
    // return ast;
}

void printTillSemanticCheckTime(char *input, char *output)
{
    // printf("%s", codefilename);
    clock_t start_time, end_time;
    double total_CPU_time, total_CPU_time_in_seconds;
    start_time = clock();
    generateSymbolTable(input, output);
    end_time = clock();
    total_CPU_time = (double)(end_time - start_time);
    total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
    printf("CPU TIME(NO OF TICKS): %lf, TOTAL CPU TIME IN SECOND: %lf\n", total_CPU_time, total_CPU_time_in_seconds);
}

int CountParseTreeNodes(ParseTreeNode *root)
{
    if (!root)
        return 0;
    int count = 1;
    ParseTreeNode *child = root->children->head;
    while (child)
    {
        count += CountParseTreeNodes(child);
        child = child->sibling;
    }
    return count;
}

int CountASTNodes(astNode *root)
{
    if (!root)
        return 0;
    int count = 1;
    astNode *child = root->children;
    while (child)
    {
        count += CountASTNodes(child);
        child = child->sibling;
    }
    return count;
}

void findActivationRecordSize(symbolTable* globalTable)
{
    // print in bold yELLOW printing Activation Record Size
    printf("\033[1;33mPrinting Activation Record Size\033[0m\n");
    symbolTableEntry* entry;
    for (int i = 0; i < LIST_SIZE; i++)
    {
        entry = globalTable->table[i];
        if(entry)
        {
            printf("Size of %s = %d\n", entry->name, entry->info->ifModule.nested_table->printOffset);
        }
    }
}

void MemoryCompressionDetails(char *input, char *output)
{
    // initialize lookup table
    lookupTable = initializeLookupTable();
    FILE *fp = fopen(input, "r");

    if (fp == NULL)
    {
        printf("Error opening source code file.\n");
        return;
    }
    // freeMappings();
    // creating and populating buffers
    buff1 = malloc(BUFF_SIZE * sizeof(char));
    buff2 = malloc(BUFF_SIZE * sizeof(char));
    populateBuffer(fp, buff1);
    if (!readCodeCompletely)
        populateBuffer(fp, buff2);
    ParseTree *parseTree = readGrammar(output, fp);
    int parseTreeNodeCount = CountParseTreeNodes(parseTree->root);
    int size_of_parseTree = sizeof(ParseTreeNode) * parseTreeNodeCount;
    printf("\n\n\033[32mParse Tree Number of Nodes = %d, Allocated Memory = %d\033[0m\n\n\n", parseTreeNodeCount, size_of_parseTree);
    if (__SYNTAX_ERROR__)
    {
        printf("\033[31mSyntax Errors Found.. AST not generated\033[0m");
        return;
    }
    AST *ast = createAST(parseTree);
    int ASTNodeCount = CountASTNodes(ast->root);
    int size_of_AST = sizeof(astNode) * ASTNodeCount;
    printf("\n\n\033[32mAST Number of Nodes = %d, Allocated Memory = %d\033[0m\n\n", ASTNodeCount, size_of_AST);
    double compression = ((size_of_parseTree - size_of_AST) * 100) / (1.00 * size_of_parseTree);
    printf("\033[32mCompression Percentage = %lf%%\033[0m\n\n", compression);
    // FILE *fp2 = fopen(astOutput, "w");
    fseek(fp, 0, SEEK_SET);
    fclose(fp);
    // fclose(fp2);
    free(buff1);
    free(buff2);
}

void printArrayDetails(char *input, char *output)
{
    // initialize lookup table
    lookupTable = initializeLookupTable();
    initArrayList();
    FILE *fp = fopen(input, "r");

    if (fp == NULL)
    {
        printf("Error opening source code file.");
        return;
    }

    // creating and populating buffers
    // freeMappings();
    buff1 = malloc(BUFF_SIZE * sizeof(char));
    buff2 = malloc(BUFF_SIZE * sizeof(char));
    populateBuffer(fp, buff1);
    if (!readCodeCompletely)
        populateBuffer(fp, buff2);
    ParseTree *parseTree = readGrammar(output, fp);
    if (__SYNTAX_ERROR__)
    {
        printf("\033[31mSyntax Errors Found.. Symbol Table not generated\033[0m");
        return;
    }
    AST *ast = createAST(parseTree);
    symbolTable *globalTable = createSymbolTable(NULL, NULL);
    symbolTable *defTable = createSymbolTable(NULL, NULL);
    printf("Checking Semantic Errors...\n");
    PopulateSymbolTable(ast->root, globalTable, defTable);
    semanticCheck(ast->root, globalTable, globalTable, defTable);
    printArrayList();
    // printST(globalTable, "global", 0);
    if (__SEMANTIC_ERROR__)
        printf("\033[31mSemantic Errors Found...\033[0m\n");
    else
        printf("\033[32mCode Compiles Successfully...\033[0m\n");

    fseek(fp, 0, SEEK_SET);
    fclose(fp);
    free(buff1);
    free(buff2);
}

void printActivationRecordSize(char *input, char *output)
{
    // initialize lookup table
    lookupTable = initializeLookupTable();
    FILE *fp = fopen(input, "r");

    if (fp == NULL)
    {
        printf("Error opening source code file.\n");
        return;
    }
    // freeMappings();
    // creating and populating buffers
    buff1 = malloc(BUFF_SIZE * sizeof(char));
    buff2 = malloc(BUFF_SIZE * sizeof(char));
    populateBuffer(fp, buff1);
    if (!readCodeCompletely)
        populateBuffer(fp, buff2);
    ParseTree *parseTree = readGrammar(output, fp);
    if (__SYNTAX_ERROR__)
    {
        printf("\033[31mSyntax Errors Found.. AST not generated\033[0m");
        return;
    }
    AST *ast = createAST(parseTree);
    symbolTable *globalTable = createSymbolTable(NULL, NULL);
    symbolTable *defTable = createSymbolTable(NULL, NULL);
    printf("Checking Semantic Errors...\n");
    PopulateSymbolTable(ast->root, globalTable, defTable);
    semanticCheck(ast->root, globalTable, globalTable, defTable);
    findActivationRecordSize(defTable);
    fseek(fp, 0, SEEK_SET);
    fclose(fp);
    free(buff1);
    free(buff2);
}

void MainDriver(char *input, char *output, int no)
{
    // initialize lookup table
    lookupTable = initializeLookupTable();
    FILE *fp = fopen(input, "r");

    if (fp == NULL)
    {
        printf("Error opening source code file.");
        return;
    }
    buff1 = malloc(BUFF_SIZE * sizeof(char));
    buff2 = malloc(BUFF_SIZE * sizeof(char));
    populateBuffer(fp, buff1);
    if (!readCodeCompletely)
        populateBuffer(fp, buff2);
    ParseTree *parseTree = readGrammar(output, fp);
    if (__SYNTAX_ERROR__)
    {
        printf("\033[31mSyntax Errors Found.. Symbol Table not generated\033[0m");
        return;
    }
    AST *ast = createAST(parseTree);
    if(no == 3)
        generateAST(ast);
    if(no > 3)
    {
        // initArrayList();
        symbolTable *globalTable = createSymbolTable(NULL, NULL);
        symbolTable *defTable = createSymbolTable(NULL, NULL);
        printf("Checking Semantic Errors...\n");
        PopulateSymbolTable(ast->root, globalTable, defTable);
        semanticCheck(ast->root, globalTable, globalTable, defTable);
        if (__SEMANTIC_ERROR__)
            printf("\033[31mSemantic Errors Found...\033[0m\n");
        else
            printf("\033[32mCode Compiles Successfully...\033[0m\n");
        
        if(no == 5)
        {
            printST(globalTable, "global", 0);
        }
        else if(no == 6)
        {
            findActivationRecordSize(globalTable);
        }
        else if(no == 7)
        {
            printArrayList();
        }
    }

    fseek(fp, 0, SEEK_SET);
    fclose(fp);
    free(buff1);
    free(buff2);
}
