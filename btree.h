#include <stdbool.h>

#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#define MAX_KEYS 3
#define MAX_LRPOINTERS 2
#define MAX_POINTERS (MAX_KEYS + 1)

struct node {
    int numOfKeys;
    bool isLeaf;
    bool isMostLeft;
    bool isMostRight;
    int keys[MAX_KEYS];
    int freePointer;
    struct node* pointers[MAX_POINTERS];
    struct node* LRpointers[2];
    struct node* parentPointer;
};

void insertKey(int key);
struct node* createNode(bool isLeaf, bool isMostLeft, bool isMostRight);
struct node** split(struct node* leaf, int key);


void insertIntoLeaf(struct node* leaf, int key);
void insert_into_parent(struct node* left, int key, struct node* right);
void insert_into_new_root(struct node* left, int key, struct node* right);

#endif

//In an internal node of a B+tree, the keys array contains the "splitting" keys that divide the search keys between the child nodes.
//The pointers array in an internal node contains pointers to the child nodes. 

//In a leaf node of a B+tree, the keys array contains the actual values (or keys) stored in the leaf node, while the pointers array contains pointers to the actual data associated with each key.