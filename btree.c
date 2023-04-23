#include<stdlib.h>
#include <stdio.h>
#include "btree.h"

void insertionSort(struct node* node);
void printNode(struct node* node);

struct node* createNode(bool isLeaf, bool isMostLeft, bool isMostRight) {
    struct node* newNode = malloc(sizeof(struct node));
    newNode->isLeaf = isLeaf;
    newNode->isMostLeft = isMostLeft;
    newNode->isMostRight = isMostRight;
    newNode->numOfKeys = 0;
    newNode->freePointer = 0;

    for (int i = 0; i < MAX_KEYS; i++) {
        newNode->keys[i] = 0;
    }

    for (int i = 0; i < MAX_POINTERS; i++) {
        newNode->pointers[i] = NULL;
    }

    for (int i = 0; i < MAX_LRPOINTERS; i++) {
        newNode->LRpointers[i] = NULL;
    }

    return newNode;
}

void insertIntoLeaf(struct node* leaf, int key){
    if (leaf->numOfKeys == MAX_KEYS)
    {
        fprintf( stderr, "this node is full\n" );
    }else{
        printf("free pointer: %d\n",leaf->freePointer);
        printf("Inserting: %d\n",key);
        leaf->keys[leaf->freePointer] = key;

        if (leaf->freePointer < MAX_KEYS - 1)
        {
            leaf->freePointer++;
        }
        leaf->numOfKeys++;
        insertionSort(leaf);
        printNode(leaf);
        
    }
}

void insertionSort(struct node* node) {
    int key, j;

    for (int i = 0; i < node->numOfKeys; i++) {
        key = node->keys[i];
        j = i - 1;

        while (j >= 0 && node->keys[j] > key) {
            node->keys[j + 1] = node->keys[j];
            j--;
        }

        node->keys[j + 1] = key;
    }
}


void printNode(struct node* node)
{
    for (int i = 0; i < node->numOfKeys; i++)
        printf("%d ", node->keys[i]);
    printf("\n");
}