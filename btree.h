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
};

struct leafNode{
    struct node node;
    int keys[MAX_LEAF_KEYS];
    struct parentNode* parentPointer;
    struct leafNode *LRpointers[MAX_LRPOINTERS];
};

struct parentNode{
    struct node node;
    int keys[MAX_PARENT_KEYS];
    int freeChildPointer;
    struct leafNode* childPointers[MAX_POINTERS];
    struct parentNode* parentPointer;
    struct parentNode *LRpointers[MAX_LRPOINTERS];
};

struct leafNode* createLeafNode();
void split(struct leafNode* leaf, struct parentNode* parent,int key);
void insertIntoTree(struct node* root, int key);
void insertIntoLeaf(struct leafNode* leaf, int key);
void insertIntoParent(struct parentNode* parent, int key);
void removeFromLeaf(struct leafNode* leaf, bool removeFromBack);
void removeFromParent(struct parentNode* parent);
void linkNodes(struct leafNode* oldNode, struct leafNode* newNode, struct parentNode* parentNode);
struct node* getRoot(struct node* node);
bool checkNodeCapacity(struct leafNode* leaf);
void setParentKeys(struct parentNode* parent);
void printChild(struct node* node);

#endif

// void insert_into_new_root(struct leafNode* left, int key, struct leafNode* right);
// void callibrateNodes(struct leafNode* oldNode, struct leafNode* newNode, struct leafNode* parentNode);

//In an internal node of a B+tree, the keys array contains the "splitting" keys that divide the search keys between the child nodes.
//The pointers array in an internal node contains pointers to the child nodes. 

//In a leaf node of a B+tree, the keys array contains the actual values (or keys) stored in the leaf node, while the pointers array contains pointers to the actual data associated with each key.