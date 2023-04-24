#include<stdlib.h>
#include <stdio.h>
#include "btree.h"
#include <stdbool.h>
#include <math.h>

void insertionSort(int* arr, int arrSize, struct node* node);
void printNode(struct node* node);
bool checkNodeCapacity(struct node* node);

struct node* createNode(bool isLeaf, bool isMostLeft, bool isMostRight){
    struct node* newNode = malloc(sizeof(struct node));
    newNode->isLeaf = isLeaf;
    newNode->isMostLeft = isMostLeft;
    newNode->isMostRight = isMostRight;
    newNode->numOfKeys = 0;
    newNode->freePointer = 0;
    newNode->parentPointer = NULL;

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

        if(leaf->isMostLeft)
        {
            printf("most left\n");
            if (checkNodeCapacity(leaf->LRpointers[1]))
            {
                printf("1\n");
            }
            
        }else if (leaf->isMostRight)
        {
            printf("most right\n");
            if (checkNodeCapacity(leaf->LRpointers[0]))
            {
                printf("2\n");
            }

        }else{
            printf("split\n");

            if (leaf->LRpointers[0] == NULL && leaf->LRpointers[1] == NULL)
            {
                split(leaf, key);
            }else
            {
                if (checkNodeCapacity(leaf->LRpointers[0]))
                {
                    printf("3\n");
                }else if (checkNodeCapacity(leaf->LRpointers[1]))
                {
                    printf("4\n");
                }else
                {
                    printf("5\n");
                }
            
            }
            
        }        
        
    }else{
        printf("free pointer: %d\n",leaf->freePointer);
        printf("Inserting: %d to %p\n",key, &leaf);
        leaf->keys[leaf->freePointer] = key;

        if (leaf->freePointer < MAX_KEYS - 1)
        {
            leaf->freePointer++;
        }
        leaf->numOfKeys++;
        insertionSort(NULL, 0,leaf);
        printNode(leaf);
        
    }
}

struct node** split(struct node* node, int key){
    int tempKeys[MAX_KEYS + 1];
    int newNodeNumOfKeys = ceil((float)MAX_KEYS/2);
    struct node** returnNodes = malloc(2 * sizeof(struct node*));

    for (int i = 0; i < MAX_KEYS; i++) { //copy keys into temp list
        tempKeys[i] = node->keys[i];
    }

    tempKeys[MAX_KEYS + 1] = key; //add last key

    insertionSort(tempKeys, MAX_KEYS + 1,NULL);

    if (node->parentPointer == NULL) // no parents
    {
        struct node* parent = createNode(0,0,0);
        struct node* newLeaf = createNode(1,0,1); //new leaf on the right
        node->isMostLeft = true; //old leaf on the left
        
        for (int i = MAX_KEYS - newNodeNumOfKeys; i < MAX_KEYS; i++) { 
            node->keys[i] = 0;
        }

        for (int i = 0; i < MAX_KEYS - newNodeNumOfKeys; i++) { 
            insertIntoLeaf(newLeaf, node->keys[i]);
        }

        returnNodes[0] = parent;
        returnNodes[1] = newLeaf;

    }
    
    return returnNodes;

}

void insertionSort(int* arr, int arrSize, struct node* node) {
    int key, j;

    if (arr != NULL && arrSize > 0) {
        for (int i = 0; i < arrSize; i++) {
            key = arr[i];
            j = i - 1;

            while (j >= 0 && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }

            arr[j + 1] = key;
        }
    }
    else if (node != NULL && node->numOfKeys > 0) {
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
}

void printNode(struct node* node){
    for (int i = 0; i < node->numOfKeys; i++)
        printf("%d ", node->keys[i]);
    printf("\n");
}

bool checkNodeCapacity(struct node* node) {
    int spaceLeft = MAX_KEYS - node->numOfKeys;
    return spaceLeft > 0;
}