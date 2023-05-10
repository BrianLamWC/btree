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
void moveKeys(struct leafNode* leaf);


struct node* getRoot(struct node* node){

    void* parent = NULL;

    if (node->isLeaf)
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
            parent = root->parentPointer;
        }else{
            parent = root;
        }
    }

    printf("Root Node is: %p\n", parent);

    return parent;
}

struct leafNode *createLeafNode(){
    struct leafNode* newLeaf = malloc(sizeof(struct leafNode));
    newLeaf->node.isMostLeft = NULL;
    newLeaf->node.isMostRight = NULL;
    newLeaf->node.freePointer = 0;
    newLeaf->node.isLeaf = true;

    for (int i = 0; i < MAX_LEAF_KEYS; i++) {
        newLeaf->keys[i] = 0;
    }

    for (int i = 0; i < MAX_LRPOINTERS; i++) {
        newLeaf->LRpointers[i] = NULL;
    }

    newLeaf->parentPointer = NULL;

    return newLeaf;
}

struct parentNode* createParentNode()
{
    struct parentNode* newParent = malloc(sizeof(struct parentNode));
    newParent->node.isMostLeft = NULL;
    newParent->node.isMostRight = NULL;
    newParent->node.freePointer = 0;
    newParent->freeChildPointer = 0;
    newParent->node.isLeaf = false;


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

void insertIntoTree(struct node* node, int key){

    if (node->isLeaf)
    {
        struct leafNode* root = (struct leafNode *)node;
        insertIntoLeaf(root, key);
    }else{
        struct parentNode* root = (struct parentNode *)node;

        int i;

        printf("keys in root : %d\n", root->node.freePointer);

        for ( i = 0; i < root->node.freePointer; i++)
        {
            if (key <= root->keys[i])
            {
                break;
            }
        }

        printf("i is %d\n",i);

        // if (i == root->node.freePointer && key >= root->keys[MAX_LEAF_KEYS - 1])
        // {
        //     i++;
        // }
        
        //printf("i is %d\n",i);

        struct node* tmpchild = (struct node*)root->childPointers[i];

        if(!tmpchild->isLeaf) 
        {
            insertIntoTree(tmpchild,key);
            return;
        }else{
            insertIntoLeaf(root->childPointers[i],key);
        }

    }

}

void insertIntoLeaf(struct leafNode* leaf, int key){
    
    if ((leaf->node.freePointer)  >= MAX_LEAF_KEYS) // leaf node is full
    {

        printf("LEAF %p is FULL\n", leaf);
        if (leaf->node.isMostLeft) // is leaf most left?
        {
            printf("most left\n");
            if (checkNodeCapacity(leaf->LRpointers[1]) && key > leaf->LRpointers[1]->keys[0])
            {
                printf("HERE\n");
                insertIntoLeaf(leaf->LRpointers[1], leaf->keys[MAX_LEAF_KEYS - 1]);
                removeFromLeaf(leaf,1);
                insertIntoLeaf(leaf,key);
            }else
            {
                split(leaf,NULL,key);
                insertIntoLeaf(leaf,key);
                printf("Spliting leaf node %p\n",leaf);
                printNode(leaf->keys, leaf->node.freePointer);
            }
            
        }
        else if (leaf->node.isMostRight) // is leaf most right?
        {
            printf("most right\n");
            if (checkNodeCapacity(leaf->LRpointers[0]))
            {
                insertIntoLeaf(leaf->LRpointers[0], leaf->keys[0]); // give lowest value
                removeFromLeaf(leaf,0);
                insertIntoLeaf(leaf,key);
            }

        }else{ // only one leaf present or it is between other leaves
            if (leaf->LRpointers[0] == NULL && leaf->LRpointers[1] == NULL) // only leaf present
            {
                split(leaf, NULL,key);
                printf("Spliting leaf node %p\n",leaf);
                printNode(leaf->keys, leaf->node.freePointer);
            }else
            {
                if (checkNodeCapacity(leaf->LRpointers[0])) // left has space
                {
                    insertIntoLeaf(leaf->LRpointers[0], key);
                }
                else if (checkNodeCapacity(leaf->LRpointers[1]))
                {
                    insertIntoLeaf(leaf->LRpointers[1], key);
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

    if ( (leaf->parentPointer != NULL) && (leaf != leaf->parentPointer->childPointers[0]) )
    {
        setParentKeys(leaf->parentPointer);
    }
    
}

void removeFromLeaf(struct leafNode* leaf, bool removeFromBack){

    if (removeFromBack)
    {
        leaf->keys[leaf->node.freePointer - 1] = 0;
        leaf->node.freePointer--;
    }else{
        leaf->keys[0] = 0;
        moveKeys(leaf);
        leaf->node.freePointer--;
    }

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
            struct parentNode *parent = createParentNode();
            struct leafNode *newLeaf = createLeafNode(); 
            linkNodes(leaf, newLeaf, parent);

            for (int i = 0; i < newNodeNumOfKeys; i++)
            {
                insertIntoLeaf(newLeaf, tempKeys[(MAX_LEAF_KEYS)-i]);
            }

            for (int i = 0; i < newNodeNumOfKeys - 1; i++)
            {
                removeFromLeaf(leaf,1);
            }

        }else{ // has parent
            if (leaf->parentPointer->node.freePointer >= MAX_PARENT_KEYS)
            {

                printf("split parents\n");

            }else{
                
                if (leaf->node.isMostLeft)
                {
                    struct leafNode *newLeaf = createLeafNode(0, 0);
                    linkNodes(leaf, newLeaf, leaf->parentPointer);
                    for (int i = 0; i < newNodeNumOfKeys; i++)
                    {
                        insertIntoLeaf(newLeaf, tempKeys[(MAX_LEAF_KEYS)-i]);
                    }
                    for (int i = 0; i < newNodeNumOfKeys - 1; i++)
                    {
                        removeFromLeaf(leaf,1);
                    }

                    

                }
                
            }
            
        }
    }

}

void linkNodes(struct leafNode* oldNode, struct leafNode* newNode, struct parentNode* parentNode){



    if (oldNode->LRpointers[0] == NULL && oldNode->LRpointers[1] == NULL) // initial tree
    {   
        newNode->LRpointers[0] = oldNode;
        newNode->parentPointer = parentNode;
        oldNode->parentPointer = parentNode;
        oldNode->node.isMostLeft = true;
        newNode->node.isMostRight = true;
        parentNode->childPointers[parentNode->freeChildPointer] = oldNode;
        parentNode->freeChildPointer++;
        parentNode->childPointers[parentNode->freeChildPointer] = newNode;
        parentNode->freeChildPointer++;
        oldNode->LRpointers[1] = newNode;
    }else if (oldNode->node.isMostLeft && oldNode->LRpointers[1] != NULL) //is most left
    {
        struct leafNode* prevNode = oldNode->LRpointers[1];
        prevNode->LRpointers[0] = newNode;
        newNode->LRpointers[1] = prevNode;
        newNode->LRpointers[0] = oldNode;
        newNode->parentPointer = parentNode;
        oldNode->LRpointers[1] = newNode;

        int i;
        
        printf("fcp : %d\n",parentNode->freeChildPointer);

        for (i = 0; i < parentNode->freeChildPointer; i++)
        {
            printf("assigning %p to cp %d\n", parentNode->childPointers[i]->LRpointers[1], i+1);
            parentNode->childPointers[i+1] = parentNode->childPointers[i]->LRpointers[1];
        }

        // printf("new : %p\n",newNode);
        // printf("new left : %p\n",newNode->LRpointers[0]);
        // printf("new right : %p\n",newNode->LRpointers[1]);
        // printf("child 1 : %p\n",parentNode->childPointers[0]);
        // printf("child 2 : %p\n",parentNode->childPointers[1]);
        // printf("child 3 : %p\n",parentNode->childPointers[2]);
        


        parentNode->freeChildPointer = i + 1;

    }






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

void moveKeys(struct leafNode* leaf) {
    int i, j = 0;

    // Move all non-zero elements forward
    for (i = 0; i < leaf->node.freePointer; i++) {
        if (leaf->keys[i] != 0) {
            leaf->keys[j++] = leaf->keys[i];
        }
    }

    // Fill remaining elements with zeros
    while (j < leaf->node.freePointer) {
        leaf->keys[j++] = 0;
    }
}

void setParentKeys(struct parentNode* parent){
    int j;
    printf("parent: %p\n", parent);

    printf("no of child: %d\n", parent->freeChildPointer);

    for (j = 0; j < parent->freeChildPointer - 1; j++)
    {
        // printf("J : %d\n",j);
        // printf("J : %d\n",parent->keys[j]);
        // printf("J : %d\n",parent->childPointers[j+1]->keys[0]);
        parent->keys[j] = parent->childPointers[j+1]->keys[0];
    }

    printf("here4\n");
    
    parent->node.freePointer = j;

    //printf("no of keys: %d\n", parent->node.freePointer);

    printf("Setting keys of parent node %p\n", parent);
    printNode(parent->keys,MAX_PARENT_KEYS);
}

void printChild(struct node* node){

    struct parentNode* parent = (struct parentNode *)node;

    for (int i = 0; i < parent->freeChildPointer; i++)
        printf("%p ", parent->childPointers[i]);
    printf("\n");
}
