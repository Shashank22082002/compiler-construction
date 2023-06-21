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
#include "linkedList.h"

// Create a new list and return a pointer to it
Linkedlist *createNewList()
{
    Linkedlist *newList = (Linkedlist *) malloc(sizeof(Linkedlist));
    if (newList == NULL) {
        printf("Error: Unable to allocate memory for new LinkedList.\n");
        exit(1);
    }
    newList->count = 0;
    newList->head = NULL;
    return newList;
}

// Create a new node with the given value and return a pointer to it
Node *createNode(int value, bool isTerminal) {
    Node *newNode = (Node *) malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Error: Unable to allocate memory for new node.\n");
        exit(1);
    }
    newNode->value = value;
    newNode->isTerminal = isTerminal;
    newNode->next = NULL;
    return newNode;
}

// Insert a new node with the given value at the end of the list
void insertNode(Linkedlist* list, int value, bool isTerminal) {
    Node *newNode = createNode(value, isTerminal);
    Node* temp = list->head;
    list->count ++;
    if (temp == NULL) {
        list->head = newNode;
        return;
    }
    while (temp -> next) {
        temp = temp -> next;
    }
    temp->next = newNode;
}

// Search for a node, given the value
bool searchNode(Linkedlist *list, int value) {
    Node *current = list->head;

    while (current != NULL && current->value != value) {
        current = current->next;
    }

    if (current == NULL) {
        return false;
    }

    return true;
}

// Delete a node with given value
void deleteNode(Linkedlist *list, int value) {
    Node *current = list->head;
    Node *previous = NULL;

    while (current != NULL && current->value != value) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Error: Value not found in list.\n");
        return;
    }

    if (previous == NULL) {
        list->head = current->next;
    } else {
        previous->next = current->next;
    }

    free(current);
    list->count--;
}

void printList(Linkedlist *list) {
    Node *current = list->head;

    while (current != NULL) {
        printf("%d ", current->value);
        current = current->next;
    }
    printf("\n");
}