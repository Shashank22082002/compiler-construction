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
#include "hashmap.h"
#include "lexer.h"
#include "global.h"

bool __SYNTAX_ERROR__ = false;
bool __SEMANTIC_ERROR__ = false;
bool __RUNTIME_ERROR__ = false;
bool readCodeCompletely = false; // set to true when fp reaches EOF
int lineNo = 1;
bool noTokensLeft = false;
char *buff1, *buff2;
int forward = 0, begin = 0;
bool bufferFlush = false;
int lexLength = 0;
bool lexLengthExceeded = false;
char ch = '\0';
bool isComment = false;
STATE current = START;
HashmapEntry *lookupTable;
int BUFF_SIZE = 20;

enum STATE start(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'))
        return S1;
    else if (c >= '0' && c <= '9')
        return S2;
    else if (c == '>')
        return S8;
    else if (c == '<')
        return S12;
    else if (c == '+')
        return S16;
    else if (c == '-')
        return S17;
    else if (c == '*')
        return S18;
    else if (c == '/')
        return S22;
    else if (c == ':')
        return S23;
    else if (c == '=')
        return S25;
    else if (c == '!')
        return S27;
    else if (c == '.')
        return S29;
    else if (c == ';')
        return S31;
    else if (c == ',')
        return S32;
    else if (c == '[')
        return S33;
    else if (c == ']')
        return S34;
    else if (c == '(')
        return S35;
    else if (c == ')')
        return S36;
    else if (c == ' ' || c == '\t' || c == '\r')
        return S37;
    else if (c == '\n')
        return S39;
    else
        return TRAP;
}

enum STATE s1(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_') || (c >= '0' && c <= '9'))
        return S1;
    else
        return START;
}

enum STATE s2(char c)
{
    if (c >= '0' && c <= '9')
        return S2;
    else if (c == '.')
        return S3;
    else
        return START;
}
enum STATE s3(char c)
{
    if (c >= '0' && c <= '9')
        return S4;
    else if (c == '.')
        return START; // back track
    else
        return TRAP;
}
enum STATE s4(char c)
{
    if (c >= '0' && c <= '9')
        return S4;
    else if (c == 'E' || c == 'e')
        return S5;
    else
        return START;
}
enum STATE s5(char c)
{
    if (c >= '0' && c <= '9')
        return S6;
    else if (c == '+' || c == '-')
        return S7;
    else
        return TRAP;
}
enum STATE s6(char c)
{
    if (c >= '0' && c <= '9')
        return S6;
    else
        return START;
}
enum STATE s7(char c)
{
    if (c >= '0' && c <= '9')
        return S6;
    else
        return TRAP;
}

enum STATE s8(char c)
{
    if (c == '>')
        return S9;
    else if (c == '=')
        return S11;
    else
        return START;
}
enum STATE s9(char c)
{
    if (c == '>')
        return S10;
    else
        return START;
}
enum STATE s10(char c)
{
    return START;
}
enum STATE s11(char c)
{
    return START;
}

enum STATE s12(char c)
{
    if (c == '<')
        return S13;
    else if (c == '=')
        return S15;
    else
        return START;
}
enum STATE s13(char c)
{
    if (c == '<')
        return S14;
    else
        return START;
}
enum STATE s14(char c)
{
    return START;
}
enum STATE s15(char c)
{
    return START;
}

enum STATE s16(char c)
{
    return START;
}

enum STATE s17(char c)
{
    return START;
}

enum STATE s18(char c)
{
    if (c == '*')
        return S19;
    else
        return START;
}
enum STATE s19(char c)
{
    if (c == '\n')
        return S38;
    else if (c == '*')
        return S20;
    else
        return S19;
}
enum STATE s20(char c)
{
    if (c == '\n')
        return S38;
    else if (c == '*')
        return S21;
    else
        return S19;
}
enum STATE s21(char c)
{
    return START;
}
enum STATE s38(char c)
{
    if (c == '\n')
        return S38;
    else if (c == '*')
        return S20;
    else
        return S19;
}

enum STATE s22(char c)
{
    return START;
}

enum STATE s23(char c)
{
    if (c == '=')
        return S24;
    else
        return START;
}
enum STATE s24(char c)
{
    return START;
}

enum STATE s25(char c)
{
    if (c == '=')
        return S26;
    else
        return TRAP;
}
enum STATE s26(char c)
{
    return START;
}

enum STATE s27(char c)
{
    if (c == '=')
        return S28;
    else
        return TRAP;
}
enum STATE s28(char c)
{
    return START;
}

enum STATE s29(char c)
{
    if (c == '.')
        return S30;
    else
        return TRAP;
}
enum STATE s30(char c)
{
    return START;
}

enum STATE s31(char c)
{
    return START;
}

enum STATE s32(char c)
{
    return START;
}

enum STATE s33(char c)
{
    return START;
}

enum STATE s34(char c)
{
    return START;
}

enum STATE s35(char c)
{
    return START;
}

enum STATE s36(char c)
{
    return START;
}

enum STATE s37(char c)
{
    return START;
}

enum STATE s39(char c)
{
    return START;
}

enum STATE trap(char c)
{
    return START;
}

enum STATE nextState(enum STATE state, char c)
{
    switch (state)
    {
    case START:
        return start(c);
        break;
    case S1:
        return s1(c);
        break;
    case S2:
        return s2(c);
        break;
    case S3:
        return s3(c);
        break;
    case S4:
        return s4(c);
        break;
    case S5:
        return s5(c);
        break;
    case S6:
        return s6(c);
        break;
    case S7:
        return s7(c);
        break;
    case S8:
        return s8(c);
        break;
    case S9:
        return s9(c);
        break;
    case S10:
        return s10(c);
        break;
    case S11:
        return s11(c);
        break;
    case S12:
        return s12(c);
        break;
    case S13:
        return s13(c);
        break;
    case S14:
        return s14(c);
        break;
    case S15:
        return s15(c);
        break;
    case S16:
        return s16(c);
        break;
    case S17:
        return s17(c);
        break;
    case S18:
        return s18(c);
        break;
    case S19:
        return s19(c);
        break;
    case S20:
        return s20(c);
        break;
    case S21:
        return s21(c);
        break;
    case S22:
        return s22(c);
        break;
    case S23:
        return s23(c);
        break;
    case S24:
        return s24(c);
        break;
    case S25:
        return s25(c);
        break;
    case S26:
        return s26(c);
        break;
    case S27:
        return s27(c);
        break;
    case S28:
        return s28(c);
        break;
    case S29:
        return s29(c);
        break;
    case S30:
        return s30(c);
        break;
    case S31:
        return s31(c);
        break;
    case S32:
        return s32(c);
        break;
    case S33:
        return s33(c);
        break;
    case S34:
        return s34(c);
        break;
    case S35:
        return s35(c);
        break;
    case S36:
        return s36(c);
        break;
    case S37:
        return s37(c);
        break;
    case S38:
        return s38(c);
        break;
    case S39:
        return s39(c);
        break;
    case TRAP:
        return trap(c);
        break;

    default:
        printf("STATE DOESN'T EXIST!!");
        return TRAP;
    }
}

// FUNCTIONS
void populateBuffer(FILE *fp, char *buff)
{
    char ch;
    int i = 0;

    while ((i < BUFF_SIZE) && (ch = fgetc(fp)) != EOF)
    {
        buff[i++] = ch;
    }

    if (i < BUFF_SIZE)
    {
        buff[i] = EOF;
        readCodeCompletely = true;
    }
}

tokenInfo *getNextToken(FILE *fp)
{
    while (!noTokensLeft)
    {
        ch = buff1[forward];
        STATE next;

        next = nextState(current, ch);
        if (current == S19)
        {
            isComment = true;
        }
        if (current == S38)
            lineNo++;

        if (next == START)
        {
            tokenInfo *newToken = NULL;
            if (current == S3) // backtrack
            {
                if (forward == 0)
                {
                    forward = BUFF_SIZE - 1;
                    char *temp = buff1;
                    buff1 = buff2;
                    buff2 = temp;
                    bufferFlush = false;
                }
                else
                    forward--;
                current = S2;
            }

            if (!lexLengthExceeded)
            {
                newToken = addNewToken(buff1, buff2, begin, forward, current);
            }
            current = next;
            begin = forward;
            lexLength = 0;
            if (bufferFlush && (!readCodeCompletely))
            {
                populateBuffer(fp, buff2);
                bufferFlush = false;
            }
            lexLengthExceeded = false;
            isComment = false;
            if (newToken != NULL)
                return newToken;
        }
        else if (next == TRAP)
        {
            if (current != START)
            {
                if (!lexLengthExceeded)
                {
                    char lexeme[21];
                    memset(lexeme, '\0', 21);
                    if (forward <= begin)
                    {
                        int i = begin;
                        for (; i < BUFF_SIZE; i++)
                        {
                            lexeme[i - begin] = buff2[i];
                        }

                        for (int j = 0; j < forward; j++)
                        {
                            lexeme[j + i - begin] = buff1[j];
                        }
                    }
                    else
                    {
                        for (int i = begin; i < forward; i++)
                        {
                            lexeme[i - begin] = buff1[i];
                        }
                    }
                    printf("\033[31mLEXICAL ERROR: in line number %d: Unidentified Token (%s)\033[0m \n", lineNo, lexeme);
                }
                else
                    printf("\033[31mLEXICAL ERROR: in line number %d: Unidentified Token and Lexeme Length exceeded\033[0m \n", lineNo);
            }
            else if (current == START && ch != EOF)
            {
                printf("\033[31mLEXICAL ERROR: in line number %d: Unidentified Token (%c)\033[0m \n", lineNo, buff1[forward]);
                forward++;
                if (forward >= BUFF_SIZE)
                {
                    char *temp = buff1;
                    buff1 = buff2;
                    buff2 = temp;
                    if (!readCodeCompletely)
                        populateBuffer(fp, buff2);
                    forward = 0;
                }
            }
            current = START;
            lexLength = 0;
            begin = forward;
            if (bufferFlush && (!readCodeCompletely))
            {
                populateBuffer(fp, buff2);
                bufferFlush = false;
            }
            lexLengthExceeded = false;
            // isComment = false;
            if (ch == EOF)
            {
                noTokensLeft = true;
                tokenInfo *dollarToken = malloc(sizeof(tokenInfo));
                dollarToken->tokenType = DOLLAR;
                dollarToken->lexeme[0] = '$';
                dollarToken->lexeme[1] = '\0';
                dollarToken->lineNo = lineNo;

                return dollarToken;
            }
        }
        else
        {
            if (lexLength == 20 && !lexLengthExceeded && !isComment)
            {
                printf("\033[31mLEXICAL ERROR: Lexeme Length exceeded in Line %d \033[0m\n", lineNo);
                lexLengthExceeded = true;
                // printf("begin %d forward %d lexlength %d\n", begin, forward, lexLength);
                // continue;
            }

            current = next;
            forward++;
            lexLength++;
            if (bufferFlush && !readCodeCompletely && (isComment || lexLengthExceeded))
            {
                populateBuffer(fp, buff2);
                bufferFlush = false;
                begin = forward;
            }
        }

        // if buffer is completely tokenised
        if (forward >= BUFF_SIZE)
        {
            char *temp = buff1;
            buff1 = buff2;
            buff2 = temp;
            bufferFlush = true;
            forward = 0;
            if (bufferFlush && !readCodeCompletely && (isComment || lexLengthExceeded))
            {
                populateBuffer(fp, buff2);
                bufferFlush = false;
                begin = forward;
                lexLength = 0;
            }
        }
    }
}

tokenInfo *addNewToken(char *buff1, char *buff2, int begin, int forward, STATE finalState)
{
    TOKEN tokenType;
    tokenDataUnion tokData;
    tokData.ifBool = false;
    tokData.ifFloat = 0.0;
    tokData.ifInt = 0;

    switch (finalState)
    {
    case S1:
        tokenType = ID;
        break;
    case S2:
    {
        tokenType = NUM;
    }
    break;
    case S4:
        tokenType = RNUM;
        break;
    case S6:
    {
        tokenType = RNUM;
    }
    break;
    case S8:
        tokenType = GT;
        break;
    case S9:
        tokenType = ENDDEF;
        break;
    case S10:
        tokenType = DRIVERENDDEF;
        break;
    case S11:
        tokenType = GE;
        break;
    case S12:
        tokenType = LT;
        break;
    case S13:
        tokenType = DEF;
        break;
    case S14:
        tokenType = DRIVERDEF;
        break;
    case S15:
        tokenType = LE;
        break;
    case S16:
        tokenType = PLUS;
        break;
    case S17:
        tokenType = MINUS;
        break;
    case S18:
        tokenType = MUL;
        break;
    case S21:
        return NULL;
    case S22:
        tokenType = DIV;
        break;
    case S23:
        tokenType = COLON;
        break;
    case S24:
        tokenType = ASSIGNOP;
        break;
    case S26:
        tokenType = EQ;
        break;
    case S28:
        tokenType = NE;
        break;
    case S30:
        tokenType = RANGEOP;
        break;
    case S31:
        tokenType = SEMICOL;
        break;
    case S32:
        tokenType = COMMA;
        break;
    case S33:
        tokenType = SQBO;
        break;
    case S34:
        tokenType = SQBC;
        break;
    case S35:
        tokenType = BO;
        break;
    case S36:
        tokenType = BC;
        break;
    case S37:
        return NULL;
    case S39:
        lineNo++;
        return NULL;
    default:
        printf("Unable to add lexeme on line number %d %d\n", lineNo, finalState);
        return NULL;
    }

    char lexeme[20+1];
    memset(lexeme, '\0', 20+1);
    if (forward <= begin)
    {
        int i = begin;
        for (; i < BUFF_SIZE; i++)
        {
            lexeme[i - begin] = buff2[i];
        }

        for (int j = 0; j < forward; j++)
        {
            lexeme[j + i - begin] = buff1[j];
        }
    }
    else
    {
        for (int i = begin; i < forward; i++)
        {
            lexeme[i - begin] = buff1[i];
        }
    }

    // printTokenName(tokenType);
    // printf(" %s", lexeme);
    // printf("\n");

    tokenInfo *newToken = malloc(sizeof(tokenInfo));
    strcpy(newToken->lexeme, lexeme);
    newToken->lineNo = lineNo;
    newToken->tokenType = tokenType;

    if (tokenType == ID && (tokenType = lookupToken(lexeme)) != -1)
    {
        newToken->tokenType = tokenType;
    }

    if (tokenType == TRUE)
    {
        newToken->tokenData.ifBool = true;
    }

    if (tokenType == FALSE)
    {
        newToken->tokenData.ifBool = false;
    }

    if (tokenType == NUM)
    {
        newToken->tokenData.ifInt = atoi(lexeme);
    }
    else if (tokenType == RNUM)
    {
        newToken->tokenData.ifFloat = atof(lexeme);
    }
    return newToken;
}

char *getTokenName(TOKEN tokenType)
{
    switch (tokenType)
    {
    case ID:
        return strdup("ID");
        break;
    case NUM:
        return strdup("NUM");
        break;
    case RNUM:
        return strdup("RNUM");
        break;
    case GT:
        return strdup("GT");
        break;
    case ENDDEF:
        return strdup("ENDDEF");
        break;
    case DRIVERENDDEF:
        return strdup("DRIVERENDDEF");
        break;
    case GE:
        return strdup("GE");
        break;
    case LT:
        return strdup("LT");
        break;
    case DEF:
        return strdup("DEF");
        break;
    case DRIVERDEF:
        return strdup("DRIVERDEF");
        break;
    case LE:
        return strdup("LE");
        break;
    case PLUS:
        return strdup("PLUS");
        break;
    case MINUS:
        return strdup("MINUS");
        break;
    case MUL:
        return strdup("MUL");
        break;
    case DIV:
        return strdup("DIV");
        break;
    case ASSIGNOP:
        return strdup("ASSIGNOP");
        break;
    case EQ:
        return strdup("EQ");
        break;
    case NE:
        return strdup("NE");
        break;
    case RANGEOP:
        return strdup("RANGEOP");
        break;
    case SEMICOL:
        return strdup("SEMICOL");
        break;
    case COMMA:
        return strdup("COMMA");
        break;
    case COLON:
        return strdup("COLON");
        break;
    case SQBO:
        return strdup("SQBO");
        break;
    case SQBC:
        return strdup("SQBC");
        break;
    case BO:
        return strdup("BO");
        break;
    case BC:
        return strdup("BC");
        break;
    case TRUE:
        return strdup("TRUE");
        break;
    case FALSE:
        return strdup("FALSE");
        break;
    case AND:
        return strdup("AND");
        break;
    case OR:
        return strdup("OR");
        break;
    case INTEGER:
        return strdup("INTEGER");
        break;
    case REAL:
        return strdup("REAL");
        break;
    case BOOLEAN:
        return strdup("BOOLEAN");
        break;
    case OF:
        return strdup("OF");
        break;
    case ARRAY:
        return strdup("ARRAY");
        break;
    case START_TK:
        return strdup("START_TK");
        break;
    case END:
        return strdup("END");
        break;
    case DECLARE:
        return strdup("DECLARE");
        break;
    case MODULE:
        return strdup("MODULE");
        break;
    case DRIVER:
        return strdup("DRIVER");
        break;
    case PROGRAM:
        return strdup("PROGRAM");
        break;
    case GET_VALUE:
        return strdup("GET_VALUE");
        break;
    case PRINT:
        return strdup("PRINT");
        break;
    case USE:
        return strdup("USE");
        break;
    case WITH:
        return strdup("WITH");
        break;
    case PARAMETERS:
        return strdup("PARAMETERS");
        break;
    case TAKES:
        return strdup("TAKES");
        break;
    case INPUT:
        return strdup("INPUT");
        break;
    case RETURNS:
        return strdup("RETURNS");
        break;
    case FOR:
        return strdup("FOR");
        break;
    case IN:
        return strdup("IN");
        break;
    case SWITCH:
        return strdup("SWITCH");
        break;
    case CASE:
        return strdup("CASE");
        break;
    case BREAK:
        return strdup("BREAK");
        break;
    case DEFAULT:
        return strdup("DEFAULT");
        break;
    case WHILE:
        return strdup("WHILE");
        break;
    case DOLLAR:
        return strdup("$");
        break;
    case EPSILON:
        return strdup("epsilon");
        break;
    default:
        printf("Invaid token %d", tokenType);
        return (char *)NULL;
    }
    return NULL;
}

HashmapEntry *initializeLookupTable()
{
    HashmapEntry *list = (HashmapEntry *)calloc(LIST_SIZE, sizeof(HashmapEntry));

    insert(list, "true", TRUE);
    insert(list, "false", FALSE);
    insert(list, "AND", AND);
    insert(list, "OR", OR);
    insert(list, "integer", INTEGER);
    insert(list, "real", REAL);
    insert(list, "boolean", BOOLEAN);
    insert(list, "of", OF);
    insert(list, "array", ARRAY);
    insert(list, "start", START_TK);
    insert(list, "end", END);
    insert(list, "declare", DECLARE);
    insert(list, "module", MODULE);
    insert(list, "driver", DRIVER);
    insert(list, "program", PROGRAM);
    insert(list, "get_value", GET_VALUE);
    insert(list, "print", PRINT);
    insert(list, "use", USE);
    insert(list, "with", WITH);
    insert(list, "parameters", PARAMETERS);
    insert(list, "takes", TAKES);
    insert(list, "input", INPUT);
    insert(list, "returns", RETURNS);
    insert(list, "for", FOR);
    insert(list, "in", IN);
    insert(list, "switch", SWITCH);
    insert(list, "case", CASE);
    insert(list, "break", BREAK);
    insert(list, "default", DEFAULT);
    insert(list, "while", WHILE);

    return list;
}

TOKEN lookupToken(char *idStr)
{
    return search(lookupTable, idStr);
}

void printAllTokens(char* fileName, int bufferSize){
    BUFF_SIZE = bufferSize;
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL){
        printf("Error opening source code file.");
        exit(0);
    }
    fseek(fp, 0, SEEK_SET);
    // creating and populating buffers
    buff1 = malloc(BUFF_SIZE * sizeof(char));
    buff2 = malloc(BUFF_SIZE * sizeof(char));
    populateBuffer(fp, buff1);
    if (!readCodeCompletely)
        populateBuffer(fp, buff2);

    // initialize lookup table
    lookupTable = initializeLookupTable();

    // generating tokens
    char *temp[] = {"line Number:", "lexeme:", "token_name:"};
    printf("\033[34m%-21s%-21s%-21s\033[0m\n", temp[0], temp[1], temp[2]);
    do
    {
        tokenInfo *nextToken = getNextToken(fp);
       if(nextToken!=NULL){
            char stringLineNo[10];
            sprintf(stringLineNo, "%d", lineNo);
            printf("%-21s", stringLineNo);
            printf("%-21s", nextToken->lexeme);
            printf("%-21s", getTokenName(nextToken->tokenType));
            printf("\n");
        }
        
    } while (!noTokensLeft);
    int ret = fclose(fp);
    if(ret == -1)
    {
        printf("File not closed successfully\n");
        exit(0);
    }
    free(buff1);
    free(buff2);
}

void initializeGlobals(){
    readCodeCompletely = false; // set to true when fp reaches EOF
    lineNo = 1;
    noTokensLeft = false;
    forward = 0, begin = 0;
    bufferFlush = false;
    lexLength = 0;
    lexLengthExceeded = false;
    ch = '\0';
    isComment = false;
    current = START;
    BUFF_SIZE = 20;
    __SYNTAX_ERROR__ = false;
    __SEMANTIC_ERROR__ = false;
}