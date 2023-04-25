#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

int main() {

    struct leafNode* root = createLeafNode(0,0);

    insertIntoLeaf(root, 3);
    insertIntoLeaf(root, 1);
    insertIntoLeaf(root, 3);
    insertIntoLeaf(root, 3);
    return 0;
}
