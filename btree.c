#include<stdlib.h>
#include <stdio.h>
#include "btree.h"
#include <stdbool.h>
#include <math.h>

void insertionSort(int* arr, int arrSize, struct leafNode* node);
void printNode(struct leafNode* node);
bool checkNodeCapacity(struct leafNode* node);

struct leafNode *createLeafNode(bool isMostLeft, bool isMostRight){
    struct leafNode* newLeaf = malloc(sizeof(struct leafNode));
    newLeaf->isMostLeft = isMostLeft;
    newLeaf->isMostRight = isMostRight;
    newLeaf->numOfKeys = 0;
    newLeaf->freePointer = 0;

    for (int i = 0; i < MAX_LEAF_KEYS; i++) {
        newLeaf->keys[i] = 0;
    }

    for (int i = 0; i < MAX_LRPOINTERS; i++) {
        newLeaf->LRpointers[i] = NULL;
    }

    newLeaf->parentPointer = NULL;

    return newLeaf;
}

struct parentNode* createParentNode(bool isMostLeft, bool isMostRight)
{
    struct parentNode* newParent = malloc(sizeof(struct parentNode));
    newParent->isMostLeft = isMostLeft;
    newParent->isMostRight = isMostRight;
    newParent->numOfKeys = 0;
    newParent->freePointer = 0;

    for (int i = 0; i < MAX_PARENT_KEYS; i++)
    {
        newParent->keys[i] = 0;
    }

    for (int i = 0; i < MAX_POINTERS; i++)
    {
        newParent->pointers[i] = NULL;
    }

    return newParent;
}

void insertIntoLeaf(struct leafNode* leaf, int key){

    if (leaf->numOfKeys == MAX_LEAF_KEYS)
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
                split(leaf, NULL,key);
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

        if (leaf->freePointer < MAX_LEAF_KEYS - 1)
        {
            leaf->freePointer++;
        }
        leaf->numOfKeys++;
        insertionSort(NULL, 0,leaf);
        printNode(leaf);
        
    }
}

void split(struct leafNode* leaf, struct parentNode* parent,int key){

    if (parent == NULL)
    {
        int tempKeys[MAX_LEAF_KEYS + 1];
        int newNodeNumOfKeys = ceil((float)MAX_LEAF_KEYS / 2);

        for (int i = 0; i < MAX_LEAF_KEYS; i++)
        { // copy keys into temp list
            tempKeys[i] = leaf->keys[i];
        }

        tempKeys[MAX_LEAF_KEYS + 1] = key; // add last key

        insertionSort(tempKeys, MAX_LEAF_KEYS + 1, NULL);

        if (leaf->parentPointer == NULL) // no parents
        {
            struct parentNode *parent = createParentNode(0, 0);
            struct leafNode *newLeaf = createLeafNode(0, 1); // new leaf on the right
            leaf->isMostLeft = true;                          // old leaf on the left

            for (int i = MAX_LEAF_KEYS - newNodeNumOfKeys; i < MAX_LEAF_KEYS; i++)
            {
                leaf->keys[i] = 0;
            }

            for (int i = 0; i < MAX_LEAF_KEYS - newNodeNumOfKeys; i++)
            {
                insertIntoLeaf(newLeaf, leaf->keys[i]);
            }

            //callibrateNodes(leaf, newLeaf, parent);
        }
    }

}

// void callibrateNode(struct node* oldNode, struct node* newNode, struct node* parentNode){

//     if (parentNode == NULL)
//     {
//         printf("no parent Node\n");
//     }else{
//         parentNode->keys[1];
//     }
    

// }

void insertionSort(int* arr, int arrSize, struct leafNode* leaf) {
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
    else if (leaf != NULL && leaf->numOfKeys > 0) {
        for (int i = 0; i < leaf->numOfKeys; i++) {
            key = leaf->keys[i];
            j = i - 1;

            while (j >= 0 && leaf->keys[j] > key) {
                leaf->keys[j + 1] = leaf->keys[j];
                j--;
            }

            leaf->keys[j + 1] = key;
        }
    }
}

void printNode(struct leafNode* node){
    for (int i = 0; i < node->numOfKeys; i++)
        printf("%d ", node->keys[i]);
    printf("\n");
}

bool checkNodeCapacity(struct leafNode* leaf){
    int spaceLeft = MAX_LEAF_KEYS - leaf->numOfKeys;
    return spaceLeft > 0;
}