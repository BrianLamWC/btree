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
        if (node->parentPointer != NULL)
        {
            setParentKeys(node->parentPointer);
        }
    }else{
        struct parentNode* parent = (struct parentNode *)node;
        int i;
        printNode(parent->keys,MAX_PARENT_KEYS);
        for ( i = 0; i < parent->node.freePointer; i++)
        {
            if (key <= parent->keys[i])
            {
                break;
            }
        }
        struct node* tmpchild = parent->childPointers[i];
        printf("tmpChild : %p\n", tmpchild);
        if(!tmpchild->isLeaf) 
        {
            insertIntoTree(tmpchild,key,root);
            return;
        }else{
            insertIntoLeaf((struct leafNode* )parent->childPointers[i],key);
            if (root->parentPointer != NULL)
            {
                setParentKeys(root->parentPointer);
                
            }else{
                setParentKeys(root);
            }
            
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
            if (checkNodeCapacity(leaf->LRpointers[1]))
            {
                printf("right has space\n");
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
            printf("most right\n");
            if (checkNodeCapacity(leaf->LRpointers[0]))
            {
                printf("left has space\n");
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
                //printf("leaf %p free pointer: %d\n",leaf,leaf->node.freePointer);
                printNode(leaf->keys, leaf->node.freePointer);
            }else
            {
                printf("mid\n");
                if (checkNodeCapacity(leaf->LRpointers[0])) // left has space
                {
                    printf("left has space\n");
                    insertIntoLeaf(leaf->LRpointers[0], leaf->keys[0]); // give lowest value
                    removeFromLeaf(leaf,0);
                    insertIntoLeaf(leaf,key);
                }
                else if (checkNodeCapacity(leaf->LRpointers[1]))
                {
                    printf("right has space\n");
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
        
    }else{
        printf("free pointer: %d\n", leaf->node.freePointer);
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
    { // copy keys into temp list
        tempKeys[i] = leaf->keys[i];
    }

    tempKeys[MAX_LEAF_KEYS] = key; // add last key

    insertionSort(tempKeys, MAX_LEAF_KEYS + 1, NULL);

    if (leaf->node.parentPointer == NULL) // no parent
    {
        struct parentNode *parent = createParentNode();
        struct leafNode *newLeaf = createLeafNode(); 
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

    }else{ // has parent
        struct parentNode* tmpParent = (struct parentNode*)leaf->node.parentPointer;
        if (tmpParent->freeChildPointer >= MAX_POINTERS)
        { 
            struct leafNode *newLeaf = createLeafNode(0, 0);
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
            splitParents((struct node*)leaf, (struct node*)newLeaf);
        }else{
            struct leafNode *newLeaf = createLeafNode(0, 0);
            linkNodes2Parents(leaf, newLeaf, (struct parentNode*)leaf->node.parentPointer);
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
        }
        
    }
    

}

void splitParents(struct node* oldNode, struct node* newNode){
    struct parentNode* oldParent = (struct parentNode*)oldNode->parentPointer;
    struct parentNode* newParent = createParentNode();

    int newParentNumOfNode = ceil((float)(MAX_POINTERS + 1)/ 2);
    int remainder;
    struct node* tmpChilds[MAX_POINTERS+1];

    //printf("newParentNumOfNode: %d\n", newParentNumOfNode);
    

    if (newParentNumOfNode % 2 == 0)
    {
        remainder = newParentNumOfNode;
    }else{
        remainder = newParentNumOfNode - 1;
    }
    
    for (int i = 0; i < MAX_POINTERS ; i++)
    {
        tmpChilds[i] = oldParent->childPointers[i];
    }
    
    //printf("remainder: %d\n", remainder);

    tmpChilds[MAX_POINTERS] = newNode;

    if (oldParent->node.parentPointer == NULL)//initial
    {
        struct parentNode* newRoot = createParentNode();
        linkParents(oldParent, newParent, newRoot);

        for (int i = 0; i < newParentNumOfNode; i++)
        {
            newParent->childPointers[i] = tmpChilds[remainder + i];
            newParent->freeChildPointer++;
        }
        for (int i = 0; i < remainder; i++)
        {
            oldParent->childPointers[(oldParent->freeChildPointer - 1 )- i] = NULL;
        }
        oldParent->freeChildPointer = remainder;
    }else{
        struct parentNode* tmpParent = (struct parentNode*)oldNode->parentPointer;
        if (tmpParent->freeChildPointer >= MAX_POINTERS)
        {
            //splitParents();
        }else{
            linkParents(oldParent,newParent,(struct parentNode*)oldParent->node.parentPointer);
        }
        
    }
    
}

void linkParents(struct parentNode* oldParent, struct parentNode* newParent, struct parentNode* newRoot){
    if (oldParent->LRpointers[0] == NULL && oldParent->LRpointers[1] == NULL) // initial tree
    {   
        newParent->LRpointers[0] = oldParent;
        newParent->node.parentPointer = (struct node*)newRoot;
        newParent->node.isMostRight = true;
        oldParent->node.parentPointer = (struct node*)newRoot;
        oldParent->node.isMostLeft = true;
        oldParent->LRpointers[1] = newParent;
        newRoot->childPointers[newRoot->freeChildPointer] = (struct node*)oldParent;
        newRoot->freeChildPointer++;
        newRoot->childPointers[newRoot->freeChildPointer] = (struct node*)newParent;
        newRoot->freeChildPointer++;
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
        for (i = 0; i < parentNode->freeChildPointer; i++)
        {
            struct leafNode* tmpChild = (struct leafNode*)parentNode->childPointers[i];
            parentNode->childPointers[i+1] = (struct node*)tmpChild->LRpointers[1];
        }
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
    return spaceLeft;
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

void setParentKeys(struct node* node){
    struct parentNode* tmpParent = (struct parentNode*)node;
    struct parentNode* orgParent = (struct parentNode*)node;
    //printf("parent node: %p free pointers : %d\n",tmpParent,tmpParent->freeChildPointer);

    for (int i = 0; i < MAX_PARENT_KEYS; i++)
    {
        if (orgParent->childPointers[i+1] != NULL)
        {
            if (!orgParent->childPointers[i+1]->isLeaf)
            {
                for (int i = 0; i < orgParent->freeChildPointer; i++)
                {
                    setParentKeys(tmpParent->childPointers[i]);
                }

                tmpParent = (struct parentNode*)tmpParent->childPointers[i+1];

                while (!tmpParent->childPointers[0]->isLeaf)
                {
                    tmpParent = (struct parentNode*)tmpParent->childPointers[0];
                    
                    printf("TRAVERSED\n");
                }

                struct leafNode* tempChild = (struct leafNode*)tmpParent->childPointers[0];

                orgParent->keys[i] = tempChild->keys[0];

            }else{
                for (int j = 0; j < orgParent->freeChildPointer - 1; j++)
                {
                    struct leafNode* tmpChild = (struct leafNode*)orgParent->childPointers[j+1];
                    if (tmpChild != NULL)
                    {
                        orgParent->keys[j] = tmpChild->keys[0];
                    }else{
                        
                        orgParent->keys[j] = 0;
                    }
                }
            }
        }else{
            orgParent->keys[i] = 0;
        }
        orgParent->node.freePointer = MAX_PARENT_KEYS - checkParentNodeCapacity(orgParent);
    }

    printf("Setting parent key %p\n",orgParent);
    printNode(orgParent->keys,MAX_PARENT_KEYS);
    
}

void printChild(struct node* node){

    struct parentNode* parent = (struct parentNode *)node;

    for (int i = 0; i < parent->freeChildPointer; i++)
        printf("%p ", parent->childPointers[i]);
    printf("\n");
}
