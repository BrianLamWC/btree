#include <stdbool.h>

#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#define MAX_LEAF_KEYS 3
#define MAX_PARENT_KEYS 3
#define MAX_LRPOINTERS 2
#define MAX_POINTERS (MAX_PARENT_KEYS + 1)

struct node
{
    bool isMostLeft;
    bool isMostRight;
    int freePointer;
    bool isLeaf;
    struct node* parentPointer;
};

struct leafNode{
    struct node node;
    int keys[MAX_LEAF_KEYS];
    struct leafNode *LRpointers[MAX_LRPOINTERS];
};

struct parentNode{
    struct node node;
    int keys[MAX_PARENT_KEYS];
    int freeChildPointer;
    struct node* childPointers[MAX_POINTERS];
    struct parentNode *LRpointers[MAX_LRPOINTERS];
};

struct leafNode* createLeafNode();
void splitLeaf(struct leafNode* leaf, int key);
void splitParents(struct node* oldNode, struct node* newNode);
void insertIntoTree(struct node* node, int key, struct node* root);
void insertIntoLeaf(struct leafNode* leaf, int key);
void insertIntoParent(struct parentNode* parent, int key);
void removeFromLeaf(struct leafNode* leaf, bool removeFromBack);
void removeFromParent(struct parentNode* parent);
void linkNodes2Parents(struct leafNode* oldNode, struct leafNode* newNode, struct parentNode* parentNode);
void linkParents(struct parentNode* oldParent, struct parentNode* newParent, struct parentNode* newRoot);
struct node* getRoot(struct node* node);
bool checkNodeCapacity(struct leafNode* leaf);
void setParentKeys(struct node* parent);
void printChild(struct node* node);
int checkParentNodeCapacity(struct parentNode* parent);


#endif
