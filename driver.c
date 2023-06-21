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
#include "removeComment.h"
#include "ast.h"
#include "symbolTable.h"
#include "codegen.h"

int main(int argc, char* argv[])
{

    char choice[50];
    BUFF_SIZE = 100;
    testfile = (char*)(malloc(sizeof(char)*(strlen(argv[1])+1)));
    strcpy(testfile, argv[1]);
    asmOutputFile = (char*)(malloc(sizeof(char)*(strlen(argv[2])+1)));
    strcpy(asmOutputFile, argv[2]);
    if (BUFF_SIZE < 20)
    {
        printf("\033[33mWarning: Buffersize is too Small (Enter Buffersize atleast 20 to avoid errors)!\033[0m\n");
    }

    printf("\033[32mLEVEL 4: Symbol table/type Checking/Semantic rules modules works/handled static and dynamic arrays in type checking and code generation\033[0m\n");
    printf("(a) \033[32mFIRST and FOLLOW set automated\033[0m\n");
    printf("(b) \033[32mBoth Lexer and Parser working fine\033[0m\n");
    printf("(c) \033[32mAll modules compile and work fine\033[0m\n");
    printf("(d) \033[32mWorks on all 6 given testcases as well as our custom testcases\033[0m\n");
    printf("(e) \033[32mParse Tree generated\033[0m\n");
    printf("(f) \033[32mSynchronization Set Panic Mode Recovery Heurestics implemented successfully!\033[0m\n");
    printf("(g) \033[32mColor highlighting done\033[0m\n");
    printf("(h) \033[32mAll semantic errors displayed properly(including while, for, switch, and so on)\033[0m\n");
    printf("(i) \033[32mAST printed in proper tree structure in a file and console\033[0m\n");
    printf("(j) \033[32mASM code generated properly\033[0m\n");
    printf("    \033[32mHARDWORK DONE AT OUR BEST!! IT WAS A FUN LEARNING EXPERIENCE!!THANKYOU MAM\033[0m\n");

    do
    {
        BUFF_SIZE = 20;
        initializeGlobals();
        printf("\n\033[34mDriver Menu ::\033[0m\n");
        printf("0: \033[36mExit\033[0m\n");
        printf("1: \033[36mLexer : Print Token List\033[0m\n");
        printf("2: \033[36mProduce Parse Tree\033[0m\n");
        printf("3: \033[36mPrint AST\033[0m\n");
        printf("4: \033[36mMemory Compression in AST\033[0m\n");
        printf("5: \033[36mPrint Symbol Table\033[0m\n");
        printf("6: \033[36mPrint Activation Record Size\033[0m\n");
        printf("7: \033[36mPrint All Array Info\033[0m\n");
        printf("8: \033[36mErrors and Compilation Time\033[0m\n");
        printf("9: \033[36mCode Generation\033[0m\n");

        printf("Enter your choice: ");
        scanf("%s", choice);
        printf("\n");
        if (strlen(choice) > 1)
        {
            printf("Wrong Choice\n");
        }
        else if (choice[0] == '0')
        {
            printf("\033[36mThanks for using 'SASTA' (\033[0m");
            printf("\033[33mS\033[0m");
            printf("\033[36mhadan\033[0m");
            printf("\033[33m A\033[0m");
            printf("\033[36mkshat\033[0m");
            printf("\033[33m S\033[0m");
            printf("\033[36mhashank\033[0m");
            printf("\033[33m T\033[0m");
            printf("\033[36marak\033[0m");
            printf("\033[33m A\033[0m");
            printf("\033[36mnish) compiler\033[0m\n");
        }
        else if (choice[0] == '1')
        {
            printAllTokens(testfile, BUFF_SIZE);
        }
        else if (choice[0] == '2')
        {
            parseTheCode(testfile, treeOutput);
        }
        else if (choice[0] == '3')
        {
            MainDriver(testfile, treeOutput, 3);
        }
        else if (choice[0] == '4')
        {
            MemoryCompressionDetails(testfile, treeOutput);
        }
        else if (choice[0] == '5')
        {
            MainDriver(testfile, treeOutput, 5);
        }
        else if (choice[0] == '6')
        {
            MainDriver(testfile, treeOutput, 6);
        }
        else if(choice[0] == '7')
        {
            printArrayDetails(testfile, treeOutput);
        }
        else if(choice[0] == '8')
        {
            printTillSemanticCheckTime(testfile, treeOutput);
        }
        else if(choice[0] == '9')
        {
            generateFinalCode(testfile, treeOutput);
        }
        else
        {
            printf("Wrong Choice\n");
        }
    } while (choice[0] != '0' || strlen(choice) != 1);
    return 0;
}