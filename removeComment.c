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
#include "removeComment.h"

void removeComments(char *testcaseFile, char *cleanFile){
    FILE *input, *output;

    input = fopen(testcaseFile, "r");
    output = fopen(cleanFile, "w");

    if (input == NULL) printf("Error opening file to clean comments.\n");
    else if(output == NULL) printf("Error opening file to write clean file.\n");
    else{
        char currentChar, nextChar;

        while((currentChar = fgetc(input)) != EOF){
            if(currentChar == '*'){
                nextChar = fgetc(input);
                if(nextChar == '*'){
                    while((currentChar = fgetc(input)) !=EOF){
                        if(currentChar == '*'){
                            currentChar = fgetc(input);
                            if(currentChar == '*') break;
                            if(currentChar == '\n') fputc(currentChar, output);
                        }
                        else if(currentChar == '\n') fputc(currentChar, output);
                    }
                }
                else{
                    fputc(currentChar, output);
                    fputc(nextChar, output);
                }
            }
            else fputc(currentChar, output);
        }
    }
    fclose(input);
    fclose(output);

    printf("\n\033[35mComment Free File created.\nCheckout in CommentFreeTestcase.txt\033[0m\n");
} 

