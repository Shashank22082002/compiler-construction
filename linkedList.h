/*

//////////////////////////TEAM//////////////////////////
BITS ID             Member Name
2020A7PS0073P       Shashank Agrawal
2020A7PS0096P       Akshat Gupta
2020A7PS0129P       Anish Ghule
2020A7PS0134P       Shadan Hussain
2020A7PS1513P       Tarak P V S

*/

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct linkedlist
{
    int count;
    struct node *head;
} Linkedlist;

// Define the structure for each node in the list
typedef struct node
{
    int value;
    bool isTerminal;
    struct node *next;
    // tokenType terminalToken;
} Node;

// Function prototypes

Linkedlist *createNewList();

Node *createNode(int value, bool isTerminal);

void insertNode(Linkedlist *list, int value, bool isTerminal);

void deleteNode(Linkedlist *list, int value);

void printList(Linkedlist *list);

bool searchNode(Linkedlist *list, int value);

#endif /* LINKEDLIST_H */