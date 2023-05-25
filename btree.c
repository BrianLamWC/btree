#include<stdlib.h>
#include <stdio.h>
#include "btree.h"
#include <stdbool.h>
#include <math.h>

void insertionSort(int* arr, int arrSize, struct leafNode* leaf);
void printNode(int* arr, int size);




struct node* getRoot(struct node* node){

    void* parent = NULL;

    if (node->isLeaf)
    {
        struct leafNode* root = (struct leafNode*)node;

        if (root->node.parentPointer != NULL)
        {
            parent = root->node.parentPointer;
        }else{
            parent = root;
        }
        
    }else{
        struct parentNode* root = (struct parentNode*)node;

        if (root->node.parentPointer != NULL)
        {
            parent = root->node.parentPointer;
        }else{
            parent = root;
        }
    }

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

    newLeaf->node.parentPointer = NULL;

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

void insertIntoTree(struct node* node, int key, struct node* root){

    if (node->isLeaf) // initial leaf
    {
        insertIntoLeaf((struct leafNode *)node, key);
    }else{
        struct parentNode* parent = (struct parentNode *)node;
        int i;
        for ( i = 0; i < parent->node.freePointer; i++)
        {
            if (key <= parent->keys[i])
            {
                break;
            }
        }
        struct node* tmpchild = parent->childPointers[i];
        if(!tmpchild->isLeaf) 
        {
            insertIntoTree(tmpchild,key,root);
            return;
        }else{
            insertIntoLeaf((struct leafNode* )tmpchild,key);
            
        }

    }
}

void insertIntoLeaf(struct leafNode* leaf, int key){
    
    if ((leaf->node.freePointer)  >= MAX_LEAF_KEYS) // leaf node is full
    {
        if (leaf->node.isMostLeft) // is leaf most left?
        {
            if (checkLeafCapacity(leaf->LRpointers[1]))
            {
                if (leaf->keys[MAX_LEAF_KEYS - 1] <=  key)
                {
                    insertIntoLeaf(leaf->LRpointers[1], key);
                }else{
                    insertIntoLeaf(leaf->LRpointers[1], leaf->keys[MAX_LEAF_KEYS - 1]);
                    removeFromLeaf(leaf, 1);
                    insertIntoLeaf(leaf, key);
                }

            }else
            {
                splitLeaf(leaf, key);
                printf("Spliting leaf node %p\n",leaf);
                printNode(leaf->keys, leaf->node.freePointer);
            }
            
        }
        else if (leaf->node.isMostRight) // is leaf most right?
        {
            if (checkLeafCapacity(leaf->LRpointers[0]))
            {
                insertIntoLeaf(leaf->LRpointers[0], leaf->keys[0]); // give lowest value
                removeFromLeaf(leaf,0);
                insertIntoLeaf(leaf,key);
            }else
            {
                splitLeaf(leaf, key);
                printf("Spliting leaf node %p\n",leaf);
                printNode(leaf->keys, leaf->node.freePointer);
            }

        }else{ // only one leaf present or it is between other leaves
            if (leaf->LRpointers[0] == NULL && leaf->LRpointers[1] == NULL) // only leaf present
            {
                splitLeaf(leaf, key);
                printf("Spliting leaf node %p\n",leaf);
                printNode(leaf->keys, leaf->node.freePointer);
            }else
            {
                if (checkLeafCapacity(leaf->LRpointers[0])) // left has space
                {
                    insertIntoLeaf(leaf->LRpointers[0], leaf->keys[0]); // give lowest value
                    removeFromLeaf(leaf,0);
                    insertIntoLeaf(leaf,key);
                }
                else if (checkLeafCapacity(leaf->LRpointers[1]))
                {
                    if (leaf->keys[MAX_LEAF_KEYS - 1] <=  key)
                    {
                        insertIntoLeaf(leaf->LRpointers[1], key);
                    }else{
                        insertIntoLeaf(leaf->LRpointers[1], leaf->keys[MAX_LEAF_KEYS - 1]);
                        removeFromLeaf(leaf, 1);
                        insertIntoLeaf(leaf, key);
                    }
                }else{
                    splitLeaf(leaf, key);
                    printf("Spliting leaf node %p\n",leaf);
                    printNode(leaf->keys, leaf->node.freePointer);
                }   
            
            }
            
        } 

        setParentKeys((struct parentNode*)leaf->node.parentPointer);
        
    }else{
        printf("Inserting: %d to leaf node %p\n",key, leaf);
        leaf->keys[leaf->node.freePointer] = key;
        leaf->node.freePointer++;
        insertionSort(NULL, 0, leaf);
        printNode(leaf->keys, leaf->node.freePointer);
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

void splitLeaf(struct leafNode* leaf, int key){
        
    int tempKeys[MAX_LEAF_KEYS + 1];
    int newNodeNumOfKeys = ceil((float)(MAX_LEAF_KEYS + 1)/ 2);
    int remainder;

    if (newNodeNumOfKeys % 2 == 0)
    {
        remainder = newNodeNumOfKeys;
    }else{
        remainder = newNodeNumOfKeys - 1;
    }
    
    for (int i = 0; i < MAX_LEAF_KEYS; i++)
    { 
        tempKeys[i] = leaf->keys[i];
    }

    tempKeys[MAX_LEAF_KEYS] = key; // add last key

    insertionSort(tempKeys, MAX_LEAF_KEYS + 1, NULL);

    if (leaf->node.parentPointer == NULL) // no parent
    {
        printf("no parent\n");
        struct parentNode* parent = createParentNode();
        struct leafNode* newLeaf = createLeafNode(); 
        linkNodes2Parents(leaf, newLeaf, parent);
        for (int i = 0; i < newNodeNumOfKeys; i++)
        {
            insertIntoLeaf(newLeaf, tempKeys[(MAX_LEAF_KEYS)-i]);
        }
        for (int i = 0; i < remainder-1; i++)
        {
            removeFromLeaf(leaf,1);
        }
        for (int i = 0; i < remainder; i++)
        {
            leaf->keys[i] = tempKeys[i];
        }
        printf("atempt to set parent keys\n");
    }else{ // has parent
        struct parentNode* tmpParent = (struct parentNode*)leaf->node.parentPointer;
        struct leafNode *newLeaf = createLeafNode(0, 0);
        tmpParent->freeChildPointer++;
        linkNodes2Parents(leaf, newLeaf, tmpParent);
        for (int i = 0; i < newNodeNumOfKeys; i++)
        {
            insertIntoLeaf(newLeaf, tempKeys[(MAX_LEAF_KEYS)-i]);
        }
        for (int i = 0; i < newNodeNumOfKeys - 1; i++)
        {
            removeFromLeaf(leaf,1);
        }       
        for (int i = 0; i < remainder; i++)
        {
            leaf->keys[i] = tempKeys[i];
        } 

        if (tmpParent->freeChildPointer > MAX_POINTERS)
        { 
            passToNeighbourParents((struct node*)leaf, (struct node*)newLeaf);
        }
    }
    

}

void splitParents(struct node* oldNode, struct node* newNode){
    struct parentNode* oldParent = (struct parentNode*)oldNode->parentPointer;
    struct parentNode* newParent = createParentNode();

    int newParentNumOfNode = ceil((float)(MAX_POINTERS + 1)/ 2);
    int remainder;
    
    if (newParentNumOfNode % 2 == 0)
    {
        remainder = newParentNumOfNode;
    }else{
        remainder = newParentNumOfNode - 1;
    }

    newParent->freeChildPointer = newParentNumOfNode;
    oldParent->freeChildPointer = remainder;

    if (oldParent->node.parentPointer == NULL) //initial 
    {
        struct parentNode* newRoot = createParentNode();
        linkParents(oldParent, newParent, newRoot);
        newNode->parentPointer = (struct node*)newParent;
        for (int i = 0; i < newParentNumOfNode; i++)
        {
            newParent->childPointers[i] = oldParent->childPointers[remainder + i];
        }

        for (int i = 0; i < newParentNumOfNode; i++)
        {
            newParent->childPointers[i]->parentPointer = (struct node*)newParent;
        }

        for (int i = 0; i < newParentNumOfNode; i++)
        {
            oldParent->childPointers[remainder + i] = NULL;
        }
        setParentKeys(newParent);
        setParentKeys(newRoot);
    }else{
        struct parentNode* root = (struct parentNode*)oldParent->node.parentPointer;

        root->freeChildPointer++;
        linkParents(oldParent, newParent, root);
        newNode->parentPointer = (struct node*)newParent;
        for (int i = 0; i < newParentNumOfNode; i++)
        {
            newParent->childPointers[i] = oldParent->childPointers[remainder + i];
        }
        for (int i = 0; i < newParentNumOfNode; i++)
        {
            oldParent->childPointers[remainder + i] = NULL;
        }
        if (root->freeChildPointer > MAX_POINTERS)
        {   


            if (root->LRpointers[0] != NULL && root->LRpointers[0]->freeChildPointer < MAX_POINTERS)
            {
                passToNeighbourParents((struct node*)oldParent, (struct node*)newParent);
            }else if (root->LRpointers[1] != NULL && root->LRpointers[1]->freeChildPointer < MAX_POINTERS)
            {
                passToNeighbourParents((struct node*)oldParent, (struct node*)newParent);
            }else{
                splitParents((struct node*)oldParent, (struct node*)newParent);
            }

        }

        setParentKeys(newParent);
        setParentKeys(root);

    }

    
}

void passToNeighbourParents(struct node* oldNode, struct node* newNode){
    struct parentNode* oldParent = (struct parentNode*)oldNode->parentPointer;
    if (oldParent->LRpointers[0] != NULL && oldParent->LRpointers[0]->freeChildPointer < MAX_POINTERS){
        struct parentNode* leftNeighbour = oldParent->LRpointers[0];
        leftNeighbour->childPointers[leftNeighbour->freeChildPointer] = oldParent->childPointers[0];
        leftNeighbour->childPointers[leftNeighbour->freeChildPointer]->parentPointer = (struct node*)leftNeighbour;
        leftNeighbour->freeChildPointer++;
        oldParent->freeChildPointer--;
        int i;
        for (i = 0; i < oldParent->freeChildPointer; i++)
        {
            oldParent->childPointers[i] = oldParent->childPointers[i+1];
        }
        oldParent->childPointers[i] = NULL;
        setParentKeys(leftNeighbour);
        setParentKeys((struct parentNode*)oldParent->node.parentPointer);

    }else if(oldParent->LRpointers[1] != NULL && oldParent->LRpointers[1]->freeChildPointer < MAX_POINTERS){
        struct parentNode* rightNeighbour = oldParent->LRpointers[1];
        oldParent->childPointers[MAX_POINTERS]->parentPointer = (struct node*)rightNeighbour;
        oldParent->freeChildPointer--;
        int i;
        for (i = MAX_POINTERS - 1; i > 0; i--)
        {
            rightNeighbour->childPointers[i] = rightNeighbour->childPointers[i-1];
        }
        rightNeighbour->childPointers[0] = oldParent->childPointers[MAX_POINTERS];
        rightNeighbour->freeChildPointer++;       
        oldParent->childPointers[MAX_POINTERS] = NULL;
        setParentKeys(rightNeighbour);
        setParentKeys((struct parentNode*)oldParent->node.parentPointer);
    }else{
        splitParents(oldNode,newNode);
    }
}

void linkParents(struct parentNode* oldParent, struct parentNode* newParent, struct parentNode* root){
    if (oldParent->LRpointers[0] == NULL && oldParent->LRpointers[1] == NULL) // initial tree
    {   
        newParent->LRpointers[0] = oldParent;
        newParent->node.parentPointer = (struct node*)root;
        newParent->node.isMostRight = true;
        oldParent->node.parentPointer = (struct node*)root;
        oldParent->node.isMostLeft = true;
        oldParent->LRpointers[1] = newParent;
        root->childPointers[root->freeChildPointer] = (struct node*)oldParent;
        root->freeChildPointer++;
        root->childPointers[root->freeChildPointer] = (struct node*)newParent;
        root->freeChildPointer++;
    }else{
        if (oldParent->node.isMostLeft && oldParent->LRpointers[1] != NULL)
        {
            struct parentNode* prevParent = oldParent->LRpointers[1];
            prevParent->LRpointers[0] = newParent;
            newParent->LRpointers[1] = prevParent;
            newParent->LRpointers[0] = oldParent;
            newParent->node.parentPointer = (struct node*)root;
            oldParent->LRpointers[1] = newParent;

        }else if (oldParent->node.isMostRight && oldParent->LRpointers[0] != NULL)
        {
            oldParent->LRpointers[1] = newParent;
            oldParent->node.isMostRight = false;
            newParent->node.isMostRight = true;
            newParent->node.parentPointer = (struct node*)root;
            newParent->LRpointers[0] = oldParent;
        }else{
            struct parentNode* prevParent = oldParent->LRpointers[1];
            prevParent->LRpointers[0] = newParent;
            newParent->LRpointers[1] = prevParent;
            newParent->LRpointers[0] = oldParent;
            oldParent->LRpointers[1] = newParent;
            newParent->node.parentPointer = (struct node*)root;
        }
        
    }

    int i;
    for (i = 0; i < root->freeChildPointer-1; i++)
    {
        struct parentNode* tmpParent = (struct parentNode*)root->childPointers[i];
        root->childPointers[i+1] = (struct node*)tmpParent->LRpointers[1];
    }

}

void linkNodes2Parents(struct leafNode* oldNode, struct leafNode* newNode, struct parentNode* parentNode){

    if (oldNode->LRpointers[0] == NULL && oldNode->LRpointers[1] == NULL) // initial tree
    {   
        newNode->LRpointers[0] = oldNode;
        newNode->node.parentPointer = (struct node*)parentNode;
        oldNode->node.parentPointer = (struct node*)parentNode;
        oldNode->node.isMostLeft = true;
        newNode->node.isMostRight = true;
        parentNode->childPointers[parentNode->freeChildPointer] = (struct node*)oldNode;
        parentNode->freeChildPointer++;
        parentNode->childPointers[parentNode->freeChildPointer] = (struct node*)newNode;
        parentNode->freeChildPointer++;
        oldNode->LRpointers[1] = newNode;
    }else
    {
        
        if (oldNode->node.isMostLeft && oldNode->LRpointers[1] != NULL)
        {
            struct leafNode* prevNode = oldNode->LRpointers[1];
            prevNode->LRpointers[0] = newNode;
            newNode->LRpointers[1] = prevNode;
            newNode->LRpointers[0] = oldNode;
            newNode->node.parentPointer = (struct node*)parentNode;
            oldNode->LRpointers[1] = newNode;

        }else if (oldNode->node.isMostRight && oldNode->LRpointers[0] != NULL)
        {
            oldNode->LRpointers[1] = newNode;
            oldNode->node.isMostRight = false;
            newNode->node.isMostRight = true;
            newNode->node.parentPointer = (struct node*)parentNode;
            newNode->LRpointers[0] = oldNode;
        }else{
            struct leafNode* prevNode = oldNode->LRpointers[1];
            prevNode->LRpointers[0] = newNode;
            newNode->LRpointers[1] = prevNode;
            newNode->LRpointers[0] = oldNode;
            oldNode->LRpointers[1] = newNode;
            newNode->node.parentPointer = (struct node*)parentNode;
        }
        
        int i;
        for (i = 0; i < parentNode->freeChildPointer-1; i++)
        {
            struct leafNode* tmpChild = (struct leafNode*)parentNode->childPointers[i];
            parentNode->childPointers[i+1] = (struct node*)tmpChild->LRpointers[1];
        }
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

bool checkLeafCapacity(struct leafNode* leaf){
    int spaceLeft = MAX_LEAF_KEYS - leaf->node.freePointer;
    return spaceLeft > 0;
}

int checkParentNodeCapacity(struct parentNode* parent){
    int spaceLeft = 0;
    for (int i = 0; i < MAX_PARENT_KEYS; i++)
    {
        if (parent->keys[i] == 0)
        {
            spaceLeft++;
        }
        
    }
    return spaceLeft;
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

void printTree(struct node* node){

    if(!node->isLeaf)
    {
        struct parentNode* parent = (struct parentNode*)node;
        struct node* nextChild = (struct node*)parent->childPointers[0];
        while (parent!= NULL)
        {   
            printf("[ ");
            for (int i = 0; i < MAX_PARENT_KEYS; i++)
            {
                printf("%d ",parent->keys[i]);
            }
            printf("]");
            parent = parent->LRpointers[1];
            
        }
        printf("\n");
        printTree(nextChild);
        
    }else{
        struct leafNode* leaf = (struct leafNode*)node;
        while (leaf!= NULL)
        {   
            printf("[ ");
            for (int i = 0; i < MAX_LEAF_KEYS; i++)
            {
                printf("%d ",leaf->keys[i]);
            }    
            printf("]");
            leaf = leaf->LRpointers[1];
        }
        printf("\n");
    }
    
}

void setParentKeys(struct parentNode* parent){
    struct parentNode* child;
    
    for (int i = 0; i < MAX_PARENT_KEYS; i++)
    {
        child = parent;
        if (parent->childPointers[i+1] != NULL)
        {
            
            if (!parent->childPointers[i+1]->isLeaf)
            {
                child = (struct parentNode*)parent->childPointers[i+1];
                while (!child->childPointers[0]->isLeaf)
                {
                    child = (struct parentNode*)child->childPointers[0];
                }
                struct leafNode* leaf = (struct leafNode*)child->childPointers[0];
                parent->keys[i] = leaf->keys[0];
            }else{
                struct leafNode* leaf = (struct leafNode*)child->childPointers[i+1];
                parent->keys[i] = leaf->keys[0];
            }

        }else{
            parent->keys[i] = 0;
        }


    }

    parent->node.freePointer = MAX_PARENT_KEYS - checkParentNodeCapacity(parent);

    printf("parent node: %p free pointer : %d free child: %d\n",parent,parent->node.freePointer,parent->freeChildPointer);
    printf("Setting parent key %p\n",parent);
    printNode(parent->keys,MAX_PARENT_KEYS);

}

void search(int key, struct node* node){
    struct parentNode* parent = NULL;
    printf("Searching for key: %d\n",key);
    printf("Travesed nodes: \n");

    while (!node->isLeaf){
        parent = (struct parentNode *)node;
        //printNode(parent->keys,MAX_PARENT_KEYS);
        int i;
        for ( i = 0; i < parent->node.freePointer; i++)
        {
            if (key < parent->keys[i]){
                break;
            }
        }
        //printf("I : %d\n",i);
        if (node->parentPointer == NULL)
        {
            printf("[ ");
            for (int a = 0; a < MAX_PARENT_KEYS; a++)
                printf("%d ", parent->keys[a]);
            printf("]");
        }else{
            struct parentNode* tmpRoot = (struct parentNode*)parent->node.parentPointer;
            //printNode(tmpRoot->keys,MAX_PARENT_KEYS);
            for ( int j = 0; j < tmpRoot->freeChildPointer; j++)
            {
                struct parentNode *tmpChild = (struct parentNode *)tmpRoot->childPointers[j];
                printf("[ ");
                for (int i = 0; i < MAX_PARENT_KEYS; i++)
                    printf("%d ", tmpChild->keys[i]);
                printf("]");
            }
        }
        node = parent->childPointers[i];

        printf("\n");
        printf("↓ traversed to child no: %d\n",i);
    }


    struct leafNode* leaf = (struct leafNode*)node;
    struct parentNode* tmpParent = (struct parentNode *)leaf->node.parentPointer;
    for (int j = 0; j < tmpParent->freeChildPointer; j++)
    {
        struct parentNode *tmpChild = (struct parentNode *)tmpParent->childPointers[j];
        printf("[ ");
        for (int i = 0; i < MAX_PARENT_KEYS; i++)
            printf("%d ", tmpChild->keys[i]);
        printf("]");
    }

    printf("\n");

    for (int k = 0; k < leaf->node.freePointer; k++)
    {
        if (key == leaf->keys[k])
        {
            printNode(leaf->keys,MAX_LEAF_KEYS);
            printf("Key found: %d Key searched: %d\n", leaf->keys[k],key);
            return;
        }
    }
    printf("Leaf node reached: []");
    printNode(leaf->keys, MAX_LEAF_KEYS);
    printf("Key not found\n");

}