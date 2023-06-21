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
#include "hashmap.h"
#include "lexer.h"
#include "parser.h"
#include "parseTree.h"
#include "global.h"
#include "time.h"
// parse the grammar
char *NoToCharMapping[LIST_SIZE];
HashmapEntry CharToNoMapping[LIST_SIZE];

bool isTerminal(char *str)
{
    int n = strlen(str);
    if (str[0] == '<' && str[n - 1] == '>')
        return 0;
    else
        return 1;
}

bool isNullable(int varTokenNumber, int epsilonTokenNo, bool *rulesChecked, int *nullables, Linkedlist *grammarRules[], int totalRules)
{
    // do recursive check for all variables in RHS.
    // is RHS is epsilon, return true
    // else call for every variable in RHS
    // if all variables in RHS are nullable, return true
    // else go to the next rule whoose head is varTokenNumber
    // if all rules involving varTokenNumber(LHS is) are checked, return false
    // printf("Checking for %d\n", varTokenNumber);
    if (nullables[varTokenNumber] != -1)
        return nullables[varTokenNumber];

    for (int i = 0; i < totalRules; i++)
    {
        if (!rulesChecked[i] && grammarRules[i]->head->value == varTokenNumber)
        {
            // printf("HERE\n");
            rulesChecked[i] = true;
            bool isNull = true;
            Node *temp = grammarRules[i]->head->next;
            while (temp != NULL)
            {
                if (temp->isTerminal && temp->value != epsilonTokenNo)
                {
                    isNull = false;
                    nullables[temp->value] = 0;
                    break;
                }
                else
                {
                    // printf("HERE\n");
                    if (!isNullable(temp->value, epsilonTokenNo, rulesChecked, nullables, grammarRules, totalRules))
                    {
                        isNull = false;
                        break;
                    }
                }
                temp = temp->next;
            }
            if (isNull)
            {
                nullables[varTokenNumber] = 1;
                return true;
            }
        }
    }
    nullables[varTokenNumber] = 0;
    return false;
}

int *FindNullables(Linkedlist *grammarRules[], int totalRules, int total_vars, int epsilonTokenNo)
{
    int *nullables = (int *)malloc(sizeof(int) * total_vars);
    for (int i = 0; i < total_vars; i++)
    {
        nullables[i] = -1;
    }
    nullables[epsilonTokenNo] = 1;
    bool *rulesChecked = (bool *)malloc(sizeof(bool) * totalRules);
    for (int i = 0; i < totalRules; i++)
    {
        rulesChecked[i] = false;
    }
    for (int i = 0; i < total_vars; i++)
    {
        nullables[i] = isNullable(i, epsilonTokenNo, rulesChecked, nullables, grammarRules, totalRules);
    }
    return nullables;
}

Linkedlist *computeFirstSet(int varTokenNo, Linkedlist *grammarRules[], Linkedlist **firstSets, int totalRules, bool *rulesChecked, int epsilonTokenNo)
{
    // printf("Computing first set for %d\n", varTokenNo);
    if (firstSets[varTokenNo] != NULL && firstSets[varTokenNo]->head != NULL)
        return firstSets[varTokenNo];
    // printf("Here 1\n");
    Linkedlist *firstSet = createNewList();
    for (int i = 0; i < totalRules; i++)
    {
        if (!rulesChecked[i] && grammarRules[i]->head->value == varTokenNo)
        {
            rulesChecked[i] = true;
            Node *temp = grammarRules[i]->head->next;
            bool isNullable = true;
            while (temp != NULL)
            {
                if (temp->isTerminal)
                {
                    // printf("In first set of %d, adding %d \n", varTokenNo, temp->value);
                    if (temp->value != epsilonTokenNo)
                    {
                        insertNode(firstSet, temp->value, temp->isTerminal);
                        isNullable = false;
                        break;
                    }
                }
                else
                {
                    Linkedlist *tempFirstSet = computeFirstSet(temp->value, grammarRules, firstSets, totalRules, rulesChecked, epsilonTokenNo);
                    // printf("Returning from first set of %d\n", temp->value);
                    Node *temp2 = tempFirstSet->head;
                    bool isEpsilonPresent = false;
                    while (temp2 != NULL)
                    {
                        if (temp2->value == epsilonTokenNo)
                            isEpsilonPresent = true;
                        else
                            insertNode(firstSet, temp2->value, temp2->isTerminal);
                        temp2 = temp2->next;
                    }
                    if (!isEpsilonPresent)
                    {
                        isNullable = false;
                        break;
                    }
                }
                temp = temp->next;
            }
            if (isNullable)
                insertNode(firstSet, epsilonTokenNo, true);
        }
    }
    firstSets[varTokenNo] = firstSet;
    // printf("First set of %d is: ", varTokenNo);
    // printList(firstSets[varTokenNo]);
    return firstSet;
}

Linkedlist **generateFirstSet(Linkedlist *grammarRules[], int totalRules, int total_vars, int epsilonTokenNo)
{
    // create a array of linkedlist to store first set of each non terminal
    Linkedlist **firstSet = (Linkedlist **)malloc(sizeof(Linkedlist *) * total_vars);
    for (int i = 0; i < total_vars; i++)
    {
        firstSet[i] = NULL;
    }
    bool *visitedRules = (bool *)malloc(sizeof(bool) * totalRules);
    for (int i = 0; i < totalRules; i++)
    {
        visitedRules[i] = false;
    }
    for (int i = 0; i < totalRules; i++)
    {
        // printf("Here rule %d", i);
        if (!visitedRules[i])
        {
            firstSet[grammarRules[i]->head->value] = computeFirstSet(grammarRules[i]->head->value, grammarRules, firstSet, totalRules, visitedRules, epsilonTokenNo);
        }
        // printf("First set of %d is ", grammarRules[i]->head->value);
        // printList(firstSet[grammarRules[i]->head->value]);
    }
    return firstSet;
}

Linkedlist *computeFollowSet(int varTokenNo, Linkedlist *grammarRules[], Linkedlist **followSet, int totalRules, int total_vars, int epsilonTokenNo, Linkedlist **firstSets, int *nullables)
{
    if (followSet[varTokenNo] != NULL)
        return followSet[varTokenNo];
    // printf("Computing follow set for %d\n", varTokenNo);
    Linkedlist *followSetOfVar = createNewList();
    for (int i = 0; i < totalRules; i++)
    {
        Node *temp = grammarRules[i]->head;
        temp = temp->next;
        while (temp != NULL)
        {
            while (temp != NULL && temp->value != varTokenNo)
            {
                temp = temp->next;
            }
            if (temp == NULL)
                continue;
            temp = temp->next;
            while (temp != NULL && nullables[temp->value] == 1)
            {
                Node *firstIterator = firstSets[temp->value]->head;
                while (firstIterator != NULL)
                {
                    if (firstIterator->value != epsilonTokenNo && !searchNode(followSetOfVar, firstIterator->value))
                        insertNode(followSetOfVar, firstIterator->value, firstIterator->isTerminal);
                    firstIterator = firstIterator->next;
                }
                temp = temp->next;
            }
            if (temp == NULL)
            {
                // compute follow of head if varTokenNo is not equal to head Value
                if (grammarRules[i]->head->value != varTokenNo)
                {
                    Linkedlist *followSetOfHead = computeFollowSet(grammarRules[i]->head->value, grammarRules, followSet, totalRules, total_vars, epsilonTokenNo, firstSets, nullables);
                    Node *followIterator = followSetOfHead->head;
                    while (followIterator != NULL)
                    {
                        if (!searchNode(followSetOfVar, followIterator->value))
                            insertNode(followSetOfVar, followIterator->value, followIterator->isTerminal);
                        followIterator = followIterator->next;
                    }
                }
            }
            else
            {
                if (temp->isTerminal)
                {
                    if (!searchNode(followSetOfVar, temp->value))
                        insertNode(followSetOfVar, temp->value, temp->isTerminal);
                }
                else
                {
                    Linkedlist *firstVarSet = firstSets[temp->value];
                    Node *firstIterator = firstVarSet->head;
                    while (firstIterator != NULL)
                    {
                        if (firstIterator->value != epsilonTokenNo && !searchNode(followSetOfVar, firstIterator->value))
                            insertNode(followSetOfVar, firstIterator->value, firstIterator->isTerminal);
                        firstIterator = firstIterator->next;
                    }
                }
            }
        }
    }
    followSet[varTokenNo] = followSetOfVar;
    // printf("Follow set of %d is: ", varTokenNo);
    // printList(followSetOfVar);
    return followSetOfVar;
}

Linkedlist **generateFollowSet(Linkedlist *grammarRules[], int totalRules, int total_vars, int epsilonTokenNo, Linkedlist **firstSets, int *nullables)
{
    // create a array of linkedlist to store first set of each non terminal
    Linkedlist **followSet = (Linkedlist **)malloc(sizeof(Linkedlist *) * total_vars);
    for (int i = 0; i < total_vars; i++)
    {
        followSet[i] = NULL;
    }

    bool *visitedRules = (bool *)malloc(sizeof(bool) * totalRules);
    for (int i = epsilonTokenNo + 1; i < total_vars; i++)
    {
        followSet[i] = computeFollowSet(i, grammarRules, followSet, totalRules, total_vars, epsilonTokenNo, firstSets, nullables);
    }

    // // print all sets
    // for (int i = 0; i < total_vars; i++) {
    //     if (followSet[i] != NULL) {
    //         printf("Follow set of %d is: ", i);
    //         printList(followSet[i]);
    //     }
    // }

    return followSet;
}

int **createParseTable(int token_no, int totalRules, int epsilonTokenNo, Linkedlist *grammarRules[], HashmapEntry variables[], char *reverseMap[], int *nullables, Linkedlist **firstSets, Linkedlist **followSets)
{
    int columns = epsilonTokenNo + 1;
    int rows = token_no - columns;
    // printf("rows: %d, columns: %d\n", rows, columns);
    int **parseTable = (int **)malloc(sizeof(int *) * rows);
    for (int i = 0; i < rows; i++)
    {
        parseTable[i] = (int *)malloc(sizeof(int) * columns);
        for (int j = 0; j < columns; j++)
        {
            parseTable[i][j] = -1;
        }
    }

    for (int i = 0; i < totalRules; i++)
    {
        Node *head = grammarRules[i]->head;
        Node *temp = grammarRules[i]->head->next;
        while (temp)
        {
            if (temp->isTerminal && temp->value != epsilonTokenNo)
            {
                parseTable[head->value - (epsilonTokenNo + 1)][temp->value] = i;
                break;
            }
            if (temp->value == epsilonTokenNo)
            {
                Linkedlist *followSet = followSets[head->value];
                Node *followIterator = followSet->head;
                while (followIterator)
                {
                    parseTable[head->value - (epsilonTokenNo + 1)][followIterator->value] = i;
                    followIterator = followIterator->next;
                }
                break;
            }
            Linkedlist *firstSetlist = firstSets[temp->value];
            Node *firstSetIterator = firstSetlist->head;
            while (firstSetIterator)
            {
                if (firstSetIterator->value != epsilonTokenNo)
                    parseTable[head->value - (epsilonTokenNo + 1)][firstSetIterator->value] = i;
                firstSetIterator = firstSetIterator->next;
            }
            if (nullables[temp->value] == 0)
                break;
            temp = temp->next;
        }
        if (temp == NULL)
        {
            Linkedlist *followSet = followSets[head->value];
            Node *followIterator = followSet->head;
            while (followIterator)
            {
                parseTable[head->value - (epsilonTokenNo + 1)][followIterator->value] = i;
                followIterator = followIterator->next;
            }
        }
    }
    // FILE* fp = fopen("parseTable.csv", "w");
    // fprintf(fp, ",");
    // for (int i = 0; i < columns-1; i++) {
    //     // if (isTerminal(reverseMap[i]) && i != epsilonTokenNo)
    //     fprintf(fp, "%s,", reverseMap[i]);
    // }
    // fprintf(fp, "\n");
    // for (int i = 0; i < rows; i++)
    // {
    //     // if (isTerminal(reverseMap[i]))
    //     //     continue;
    //     fprintf(fp,"%s,", reverseMap[i+columns]);
    //     for (int j = 0; j < columns-1; j++)
    //     {
    //         // if (!isTerminal(reverseMap[j])|| j == epsilonTokenNo)
    //         //     continue;
    //         if(parseTable[i][j] == -1)
    //         {
    //             fprintf(fp,",");
    //             continue;
    //         }
    //         Linkedlist *rule = grammarRules[parseTable[i][j]];
    //         Node *ruleIterator = rule->head;
    //         fprintf(fp, "%s -> ", reverseMap[ruleIterator->value]);
    //         ruleIterator = ruleIterator->next;
    //         while (ruleIterator) {
    //             fprintf(fp, "%s ", reverseMap[ruleIterator->value]);
    //             ruleIterator = ruleIterator->next;
    //         }
    //         fprintf(fp, ",");
    //     }
    //     fprintf(fp, "\n");
    // }
    // fclose(fp);
    return parseTable;
}

ParseTree *readGrammar(char *output, FILE *fp)
{
    FILE *fp1 = fopen("grammar.txt", "r");
    if (fp1 == NULL)
    {
        printf("Error opening grammar file \n");
        return NULL;
    }
    // assuming maximum characters in a line of grammar is 255;
    char line[256];
    char *token;
    freeMappings();
    initMappings();
    HashmapEntry variables[LIST_SIZE] = {0};
    Linkedlist *grammarRules[LIST_SIZE];
    char *reverseMap[LIST_SIZE];

    for (int i = 0; i < LIST_SIZE; i++)
    {
        reverseMap[i] = NULL;
        variables[i].pattern = NULL;
    }

    int rule_no = 0, token_no = 0;

    while (token_no <= EPSILON)
    {
        char *tokenName = getTokenName(token_no);
        char *token = malloc(sizeof(char) * (strlen(tokenName) + 1));
        strcpy(token, tokenName);
        insert(variables, token, token_no++);
        reverseMap[search(variables, token)] = (char *)malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(reverseMap[search(variables, token)], token);
    }

    while (fgets(line, sizeof(line), fp1))
    {
        Linkedlist *newRule = createNewList();
        token = strtok(line, " \n\t\r");
        if (search(variables, token) == -1)
        {
            insert(variables, token, token_no++);
            reverseMap[search(variables, token)] = (char *)malloc(sizeof(char) * (strlen(token) + 1));
            strcpy(reverseMap[search(variables, token)], token);
        }
        int varTokenNo = search(variables, token);
        insertNode(newRule, varTokenNo, isTerminal(token));
        // printf("%s(%d) ->", token, varTokenNo);
        while (1)
        {
            token = strtok(NULL, " \n\t\r");
            if (token != NULL)
            {
                if (search(variables, token) == -1)
                {
                    insert(variables, token, token_no++);
                    reverseMap[search(variables, token)] = (char *)malloc(sizeof(char) * (strlen(token) + 1));
                    strcpy(reverseMap[search(variables, token)], token);
                }
                varTokenNo = search(variables, token);
                // printf("%s(%d) ", token, varTokenNo);
                insertNode(newRule, varTokenNo, isTerminal(token));
            }
            else
                break;
        }
        // printf("\n");
        grammarRules[rule_no++] = newRule;
        // printList(newRule);
    }
    // printf("Grammar read successfully!\n");
    // printf("Total number of rules = %d, total number of tokens = %d\n", rule_no, token_no);
    // for (int i = 0; i < token_no; i++) {
    //     printf("%s -> %d\n", reverseMap[i], i);
    // }
    fclose(fp1);
    Linkedlist **firstSets = generateFirstSet(grammarRules, rule_no, token_no, search(variables, "epsilon"));
    int *nullables = FindNullables(grammarRules, rule_no, token_no, search(variables, "epsilon"));
    // for (int i = 0; i < token_no; i++) {
    //     if (nullables[i] == 1)
    //         printf("%s is nullable \n", reverseMap[i]);
    // }
    Linkedlist **followSets = generateFollowSet(grammarRules, rule_no, token_no, search(variables, "epsilon"), firstSets, nullables);

    // printf("Hello\n");
    for (int i = 0; i < LIST_SIZE; i++)
    {
        // printf("Hello2\n");
        if (reverseMap[i] != NULL)
        {
            NoToCharMapping[i] = malloc(sizeof(char) * (strlen(reverseMap[i]) + 1));

            strcpy(NoToCharMapping[i], reverseMap[i]);
            insert(CharToNoMapping, NoToCharMapping[i], i);
            // printf("%s\n", NoToCharMapping[i]);
            // printf("%s\n", reverseMap[i]);
        }
    }

    // printf("FIRST SET\n");
    // int count = 0;
    // for (int i = 0; i < token_no; i++)
    // {
    //     if(isTerminal(reverseMap[i])) {
    //         continue;
    //     }
    //     count++;
    //     printf("%d %s ",count, reverseMap[i]);
    //     Node *temp = firstSets[i]->head;
    //     while(temp) {
    //         printf("%s ", reverseMap[temp->value]);
    //         temp = temp->next;
    //     }
    //     printf("\n");
    // }
    // printf("Terminal count = %d, NonTerminal = %d\n", token_no - count, count);
    // printf("\nFOLLOW SET\n");
    // for (int i = EPSILON+1; i < token_no; i++){
    //     // if (nullables[i] == 1 && strcmp(reverseMap[i], "epsilon") != 0)
    //     // {
    //         printf("%s ", reverseMap[i]);
    //         Node *temp = followSets[i]->head;
    //         while(temp) {
    //             printf("%s ", reverseMap[temp->value]);
    //             temp = temp->next;
    //         }
    //         printf("\n");
    //     // }
    // }
    // printf("\nPARSE TABLE\n");
    int **parseTable = createParseTable(token_no, rule_no, search(variables, "epsilon"), grammarRules, variables, reverseMap, nullables, firstSets, followSets);

    // printf("PARSE TABLE COMPLETED\n");
    ParseTree *parseTree = generateParseTree(parseTable, firstSets, followSets, grammarRules, variables, reverseMap, nullables, rule_no, token_no, search(variables, "epsilon"), fp);

    // printf("Parse Tree generated successfully!\n");
    printParseTree(parseTree, output, reverseMap);

    // printParseTree(parseTree,"ParseTree.csv",reverseMap);
    return parseTree;
}

void parseTheCode(char *input, char *output)
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
    buff1 = malloc(BUFF_SIZE * sizeof(char));
    buff2 = malloc(BUFF_SIZE * sizeof(char));
    populateBuffer(fp, buff1);
    if (!readCodeCompletely)
        populateBuffer(fp, buff2);
    readGrammar(output, fp);
    fseek(fp, 0, SEEK_SET);
    fclose(fp);
    free(buff1);
    free(buff2);
    printOnConsole(output);
    // print in bold and big font for better view checkout file "output.txt"
    printf("\n\n\n");
    printf("\033[1mNOTE :: THIS output is also printed in the file \033[32m \033[36m%s\033[32m\n\033[1mPLEASE CHECK THIS FILE FOR BETTER VIEW\033[32m", output);
    printf("\n\n\n");
}

void printExecutionTime(char *output, char *codefilename)
{
    // printf("%s", codefilename);
    clock_t start_time, end_time;
    double total_CPU_time, total_CPU_time_in_seconds;
    start_time = clock();
    parseTheCode(codefilename, output);
    end_time = clock();
    total_CPU_time = (double)(end_time - start_time);
    total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
    printf("CPU TIME(NO OF TICKS): %lf, TOTAL CPU TIME IN SECOND: %lf\n", total_CPU_time, total_CPU_time_in_seconds);
}

void initMappings()
{
    for (int i = 0; i < LIST_SIZE; i++)
    {
        NoToCharMapping[i] = NULL;
        CharToNoMapping[i].digit = INT_MIN;
        CharToNoMapping[i].pattern = NULL;
    }
}

void freeMappings()
{
    for (int i = 0; i < LIST_SIZE; i++)
    {
        if (NoToCharMapping[i] != NULL)
            free(NoToCharMapping[i]);
        if (CharToNoMapping[i].pattern != NULL)
            free(CharToNoMapping[i].pattern);
    }
}
void printOnConsole(char *fileName)
{
    // take a fileName as input and print the contents of the file on the console
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Error opening file");
        return;
    }
    char c = fgetc(fp);
    while (c != EOF)
    {
        printf("%c", c);
        c = fgetc(fp);
    }
    fclose(fp);
}