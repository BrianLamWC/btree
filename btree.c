#include<stdlib.h>
#include <stdio.h>
#include "btree.h"
#include <stdbool.h>
#include <math.h>

void insertionSort(int* arr, int arrSize, struct leafNode* leaf, struct parentNode* parent);
void printNode(int* arr, int size);
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
    newParent->freeChildPointer = 0;

    for (int i = 0; i < MAX_PARENT_KEYS; i++)
    {
        newParent->keys[i] = 0;
    }

    for (int i = 0; i < MAX_POINTERS; i++)
    {
        newParent->childPointers[i] = NULL;
    }

    return newParent;
}

void insertIntoLeaf(struct leafNode* leaf, int key){

    if (leaf->numOfKeys >= MAX_LEAF_KEYS) // leaf node is full
    {

        if(leaf->isMostLeft) // is leaf most left?
        {
            printf("most left\n");
            if (checkNodeCapacity(leaf->LRpointers[1]))
            {
                insertIntoLeaf(leaf->LRpointers[1], key);
            }else{
                split() //till here 4/29
            }
            
        }else if (leaf->isMostRight) // is leaf most right?
        {
            printf("most right\n");
            if (checkNodeCapacity(leaf->LRpointers[0]))
            {
                insertIntoLeaf(leaf->LRpointers[0], key);
            }

        }else{ // only one leaf present or it is between other leaves 

            if (leaf->LRpointers[0] == NULL && leaf->LRpointers[1] == NULL) // only leaf present 
            {
                split(leaf, NULL,key);
                printNode(leaf->keys, MAX_LEAF_KEYS);
            }else
            {
                if (checkNodeCapacity(leaf->LRpointers[0])) //left has space
                {
                    insertIntoLeaf(leaf->LRpointers[0], key);
                }else if (checkNodeCapacity(leaf->LRpointers[1]))
                {
                    insertIntoLeaf(leaf->LRpointers[1], key);
                }else
                {
                    printf("5\n");
                }
            
            }
            
        }        
        
    }else{
        printf("free pointer: %d\n",leaf->freePointer);
        printf("Inserting: %d to leaf node %p\n",key, leaf);
        leaf->keys[leaf->freePointer] = key;
        leaf->freePointer++;
        leaf->numOfKeys++;
        insertionSort(NULL, 0, leaf, NULL);
        printNode(leaf->keys, leaf->numOfKeys);
        
    }
}

void removeFromLeaf(struct leafNode* leaf){
    leaf->keys[leaf->freePointer - 1] = 0;
    leaf->freePointer--;
    leaf->numOfKeys--;
}

void insertIntoParent(struct parentNode *parent, int key){
    if (parent->numOfKeys >= MAX_PARENT_KEYS)
    {
        printf("parent node full\n");
    }else{

        printf("free pointer: %d\n", parent->freePointer);
        printf("Inserting to: %d to parent node %p\n", key, parent);
        parent->keys[parent->freePointer] = key;

        if (parent->freePointer < MAX_PARENT_KEYS - 1)
        {
            parent->freePointer++;
        }
        parent->numOfKeys++;

        printNode(parent->keys, MAX_PARENT_KEYS);
    }
    
}

void removeFromParent(struct parentNode* parent){
    parent->keys[parent->freePointer - 1] = 0;
    parent->freePointer--;
    parent->numOfKeys--;
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

        insertionSort(tempKeys, MAX_LEAF_KEYS + 1, NULL, NULL);

        if (leaf->parentPointer == NULL) // no parents
        {
            struct parentNode *parent = createParentNode(0, 0);
            struct leafNode *newLeaf = createLeafNode(0, 1); // new leaf on the right
            leaf->isMostLeft = true;                          // old leaf on the left

            for (int i = 0; i < newNodeNumOfKeys; i++)
            {
                insertIntoLeaf(newLeaf, leaf->keys[(MAX_LEAF_KEYS -1 )-i]);
            }

            for (int i = 0; i < newNodeNumOfKeys; i++)
            {
                removeFromLeaf(leaf);
            }



            linkNodes(leaf, newLeaf, parent);
        }
    }

}

void linkNodes(struct leafNode* oldNode, struct leafNode* newNode, struct parentNode* parentNode){

    if (parentNode == NULL)
    {
        printf("no parent Node\n");
    }else{ //link parent new and old
        insertIntoParent(parentNode, newNode->keys[0]);
        parentNode->childPointers[parentNode->freeChildPointer] = oldNode;
        parentNode->freeChildPointer++;
        parentNode->childPointers[parentNode->freeChildPointer] = newNode;
        parentNode->freeChildPointer++;

        newNode->LRpointers[0] = oldNode;
        newNode->parentPointer = parentNode;

        oldNode->LRpointers[1] = newNode;
        oldNode->parentPointer = parentNode;
    }

}

void insertionSort(int* arr, int arrSize, struct leafNode* leaf, struct parentNode* parent) {
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
    else if (parent != NULL && parent->numOfKeys > 0)
    {
        for (int i = 0; i < parent->numOfKeys; i++)
        {
            key = parent->keys[i];
            j = i - 1;

            while (j >= 0 && parent->keys[j] > key)
            {
                parent->keys[j + 1] = parent->keys[j];
                j--;
            }

            parent->keys[j + 1] = key;
        }
    }
    
}

void printNode(int* arr, int size){
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

bool checkNodeCapacity(struct leafNode* leaf){
    int spaceLeft = MAX_LEAF_KEYS - leaf->numOfKeys;
    return spaceLeft > 0;
}