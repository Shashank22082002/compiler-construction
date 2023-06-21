/*

//////////////////////////TEAM//////////////////////////
BITS ID             Member Name
2020A7PS0073P       Shashank Agrawal
2020A7PS0096P       Akshat Gupta
2020A7PS0129P       Anish Ghule
2020A7PS0134P       Shadan Hussain
2020A7PS1513P       Tarak P V S

*/

#ifndef HASHMAP_H
#define HASHMAP_H

#define LIST_SIZE 269

typedef struct
{
    char *pattern;
    int digit;
} HashmapEntry;

// hashing implementation
int generateHashValue(char *pattern);

// quadratic probing function
int quadraticProbing(int hashValue, int i);

// insert a pattern-digit pair into the list
void insert(HashmapEntry *list, char *pattern, int digit);

// search for a pattern in the list
int search(HashmapEntry *list, char *pattern);

#endif /* HASHMAP_H */