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
    struct node* childPointers[MAX_POINTERS + 1];
    struct parentNode *LRpointers[MAX_LRPOINTERS];
};

struct leafNode* createLeafNode();
void splitLeaf(struct leafNode* leaf, int key);
void splitParents(struct node* oldNode);
void insertIntoTree(struct node* node, int key, struct node* root);
void insertIntoLeaf(struct leafNode* leaf, int key);
void removeFromLeaf(struct leafNode* leaf, bool removeFromBack);
void linkNodes2Parents(struct leafNode* oldNode, struct leafNode* newNode, struct parentNode* parentNode);
void linkParents(struct parentNode* oldParent, struct parentNode* newParent, struct parentNode* newRoot);
void passToNeighbourParents(struct node* oldNode, struct node* newNode);
struct node* getRoot(struct node* node);
bool checkNodeCapacity(struct leafNode* leaf);
void printTree(struct node* node);
int checkParentNodeCapacity(struct parentNode* parent);
void setParentKeys(struct parentNode* parent);
void search(int key,struct node* node);
void moveKeys(struct leafNode *leaf);
bool checkLeafCapacity(struct leafNode *node);

#endif
