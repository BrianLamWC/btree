#include<stdlib.h>
#include <stdio.h>
#include "btree.h"
#include <stdbool.h>
#include <math.h>

void insertionSort(int* arr, int arrSize, struct leafNode* leaf);
void printNode(int* arr, int size);
bool checkNodeCapacity(struct leafNode* node);
struct leafNode* voidToLeaf(void* leaf);
struct leafParent *voidToParent(void *leaf);

void* getRoot(void* node, bool isLeaf){

    void* parent = NULL;

    if (isLeaf)
    {
        

        struct leafNode* root = (struct leafNode*)node;

        if (root->parentPointer != NULL)
        {
            parent = root->parentPointer;
        }else{
            parent = root;
        }
        

    }else{
        struct parentNode* root = (struct parentNode*)node;

        if (root->parentPointer != NULL)
        {
            printf("here1\n");
            parent = root->parentPointer;
        }else{
            printf("here2\n");
            parent = root;
        }

    }

    printf("Root Node is: %p\n", parent);

    return parent;
}

struct leafNode *createLeafNode(bool isMostLeft, bool isMostRight){
    struct leafNode* newLeaf = malloc(sizeof(struct leafNode));
    newLeaf->node.isMostLeft = isMostLeft;
    newLeaf->node.isMostRight = isMostRight;
    newLeaf->node.freePointer = 0;

    for (int i = 0; i < MAX_LEAF_KEYS; i++) {
        newLeaf->keys[i] = 0;
    }

    for (int i = 0; i < MAX_LRPOINTERS; i++) {
        newLeaf->node.LRpointers[i] = NULL;
    }

    newLeaf->parentPointer = NULL;

    return newLeaf;
}

struct parentNode* createParentNode(bool isMostLeft, bool isMostRight)
{
    struct parentNode* newParent = malloc(sizeof(struct parentNode));
    newParent->node.isMostLeft = isMostLeft;
    newParent->node.isMostRight = isMostRight;
    newParent->node.freePointer = 0;
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

void insertIntoTree(void* rootNode, bool isLeaf, int key){

    if (isLeaf)
    {
        struct leafNode* root = (struct leafNode *)rootNode;
        insertIntoLeaf(root, key);
    }else{
        struct parentNode* root = (struct parentNode *)rootNode;

        for ( int i = 0; i < root->freeChildPointer; i++)
        {
            if (key <= root->keys[i])
            {
                insertIntoLeaf(root->childPointers[i],key);
            }
            
        }
        
    }
    

}

void insertIntoLeaf(struct leafNode* leaf, int key){
    
    if ((leaf->node.freePointer)  >= MAX_LEAF_KEYS) // leaf node is full
    {
        if (leaf->node.isMostLeft) // is leaf most left?
        {
            printf("most left\n");
            if (checkNodeCapacity(leaf->node.LRpointers[1]))
            {
                insertIntoLeaf(leaf->node.LRpointers[1], key);
            }else{
                split(leaf,NULL,key);
            }
            
        }
        else if (leaf->node.isMostRight) // is leaf most right?
        {
            printf("most right\n");
            if (checkNodeCapacity(leaf->node.LRpointers[0]))
            {
                insertIntoLeaf(leaf->node.LRpointers[0], key);
            }

        }else{ // only one leaf present or it is between other leaves

            if (leaf->node.LRpointers[0] == NULL && leaf->node.LRpointers[1] == NULL) // only leaf present
            {
                split(leaf, NULL,key);
                printf("Leaf node %p after spliting\n",leaf);
                printNode(leaf->keys, MAX_LEAF_KEYS);
            }else
            {
                if (checkNodeCapacity(leaf->node.LRpointers[0])) // left has space
                {
                    insertIntoLeaf(leaf->node.LRpointers[0], key);
                }
                else if (checkNodeCapacity(leaf->node.LRpointers[1]))
                {
                    insertIntoLeaf(leaf->node.LRpointers[1], key);
                }else
                {
                    printf("5\n");
                }
            
            }
            
        }        
        
    }else{
        
        printf("free pointer: %d\n", leaf->node.freePointer);
        printf("Inserting: %d to leaf node %p\n",key, leaf);
        leaf->keys[leaf->node.freePointer] = key;
        leaf->node.freePointer++;
        insertionSort(NULL, 0, leaf);
        printNode(leaf->keys, leaf->node.freePointer);

    }
}

void removeFromLeaf(struct leafNode* leaf){
    leaf->keys[leaf->node.freePointer - 1] = 0;
    leaf->node.freePointer--;
}

void insertIntoParent(struct parentNode *parent, int key){
    if ((parent->node.freePointer + 1) >= MAX_PARENT_KEYS)
    {
        printf("parent node full\n");
    }else{

        printf("free pointer: %d\n", parent->node.freePointer);
        printf("Inserting to: %d to parent node %p\n", key, parent);
        parent->keys[parent->node.freePointer] = key;
        parent->node.freePointer++;
        printNode(parent->keys, parent->node.freePointer);
    }
    
}

void removeFromParent(struct parentNode* parent){
    parent->keys[parent->node.freePointer - 1] = 0;
    parent->node.freePointer--;
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

        tempKeys[MAX_LEAF_KEYS] = key; // add last key

        insertionSort(tempKeys, MAX_LEAF_KEYS + 1, NULL);

        if (leaf->parentPointer == NULL) // no parent
        {
            struct parentNode *parent = createParentNode(0, 0);
            struct leafNode *newLeaf = createLeafNode(0, 0); // new leaf on the right
            //leaf->isMostLeft = true;                          // old leaf on the left

            for (int i = 0; i < newNodeNumOfKeys; i++)
            {
                insertIntoLeaf(newLeaf, tempKeys[(MAX_LEAF_KEYS)-i]);
            }

            for (int i = 0; i < newNodeNumOfKeys - 1; i++)
            {
                removeFromLeaf(leaf);
            }

            linkNodes(leaf, newLeaf, parent);

        }else{ // has parent
            if (leaf->parentPointer->node.freePointer + 1 == MAX_PARENT_KEYS)
            {

                printf("split parents\n");

            }else{

                if (leaf->node.isMostLeft)
                {
                    struct leafNode *newLeaf = createLeafNode(0, 0);

                    for (int i = 0; i < newNodeNumOfKeys; i++)
                    {
                        insertIntoLeaf(newLeaf, leaf->keys[(MAX_LEAF_KEYS - 1) - i]);
                    }

                    for (int i = 0; i < newNodeNumOfKeys; i++)
                    {
                        removeFromLeaf(leaf);
                    }

                    linkNodes(leaf, newLeaf, leaf->parentPointer);

                }
                
            }
            
        }
    }

}

void linkNodes(struct leafNode* oldNode, struct leafNode* newNode, struct parentNode* parentNode){

    if (oldNode->node.LRpointers[0] == NULL && oldNode->node.LRpointers[1] == NULL) // initial tree
    {      
        oldNode->parentPointer = parentNode;
    }

    insertIntoParent(parentNode, newNode->keys[0]);
    parentNode->childPointers[parentNode->freeChildPointer] = oldNode;
    parentNode->freeChildPointer++;
    parentNode->childPointers[parentNode->freeChildPointer] = newNode;
    parentNode->freeChildPointer++;
    newNode->node.LRpointers[0] = oldNode;
    newNode->parentPointer = parentNode;
    oldNode->node.LRpointers[1] = newNode;

}

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
    else if (leaf != NULL && leaf->node.freePointer + 1 > 0) {
        for (int i = 0; i < leaf->node.freePointer; i++) {
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

void printNode(int* arr, int size){
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

bool checkNodeCapacity(struct leafNode* leaf){
    int spaceLeft = MAX_LEAF_KEYS - leaf->node.freePointer;
    return spaceLeft > 0;
}