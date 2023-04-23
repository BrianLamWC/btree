#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

int main() {

    struct node* root = createNode(1,1,1);

    insertIntoLeaf(root, 3);
    insertIntoLeaf(root, 1);
    insertIntoLeaf(root, 3);
    return 0;
}
