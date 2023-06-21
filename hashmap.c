/*

//////////////////////////TEAM//////////////////////////
BITS ID             Member Name
2020A7PS0073P       Shashank Agrawal
2020A7PS0096P       Akshat Gupta
2020A7PS0129P       Anish Ghule
2020A7PS0134P       Shadan Hussain
2020A7PS1513P       Tarak P V S

*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"

#define LIST_SIZE 269

// hashing implementation
int generateHashValue(char *pattern ) {
    int hashValue = 0;
    for (int i = 0; i < strlen(pattern); i++) hashValue = (hashValue * 61 + pattern[i])%LIST_SIZE;
    return hashValue;
}

// quadratic probing function
int quadraticProbing(int hashValue, int i) {
    return (hashValue + i * i) % LIST_SIZE;
}

// insert a pattern-digit pair into the list
void insert(HashmapEntry *list, char *pattern, int digit) {
    int hashValue = generateHashValue(pattern), j = 0;
    while (list[quadraticProbing(hashValue, j)].pattern != NULL) j++;
    list[quadraticProbing(hashValue, j)].pattern = (char *)malloc(sizeof(char) * (strlen(pattern)+1));
    strcpy(list[quadraticProbing(hashValue, j)].pattern, pattern);
    list[quadraticProbing(hashValue, j)].digit = digit;
}

// search for a pattern in the list
int search(HashmapEntry *list, char *pattern) {
    int hashValue = generateHashValue(pattern), j = 0;
    while (list[quadraticProbing(hashValue, j)].pattern != NULL) {
        if (strcmp(list[quadraticProbing(hashValue, j)].pattern, pattern) == 0) 
            return list[quadraticProbing(hashValue, j)].digit;
        j++;
    }
    return -1; // pattern not found
}