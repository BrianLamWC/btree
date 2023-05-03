#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"

int main() {

    void* root = createLeafNode(0,0);
    int intialLeafNumOfKeys = 0;

    char input[100];
    while (1)
    {

        

        if (intialLeafNumOfKeys < MAX_LEAF_KEYS + 1)
        {
            root = getRoot(root,true);

        }else if(intialLeafNumOfKeys == MAX_LEAF_KEYS + 1)
        {
            root = getRoot(root,true);
            intialLeafNumOfKeys++;
        }
        else
        {
            root = getRoot(root, false);
        }

        printf("Enter a command: ");
        fgets(input, 100, stdin);
        input[strcspn(input, "\n")] = 0; // Remove trailing newline character

        if (strcmp(input, "quit") == 0)
        {
            printf("Exiting program...\n");
            break;
        }
        else if (strcmp(input, "help") == 0)
        {
            printf("List of available commands:\n");
            printf("  - help: Display this help message\n");
            printf("  - print: Print something\n");
            printf("  - count: Count something\n");
            printf("  - do something: Do something\n");
        }
        else if (strcmp(input, "tree") == 0)
        {
            //insertIntoTree(initialLeaf, true);
        }
        else if (strcmp(input, "insert") == 0)
        {
            int key;
            printf("Enter key to insert into the tree: ");
            scanf("%d", &key);
            getchar(); // consume newline character left in the input buffer by scanf

            

            if (intialLeafNumOfKeys <= MAX_LEAF_KEYS)
            {   
                insertIntoTree(root, true, key);
                intialLeafNumOfKeys++;
            }else{
                insertIntoTree(root, false, key);
                printf("%d %p\n",checkNodeCapacity(root), root);
            }

        }
        else if (strcmp(input, "delete") == 0)
        {
            printf("deleting key\n");
        }
        else if (strcmp(input, "search") == 0)
        {
            printf("searching key\n");
        }
        else
        {
            printf("Invalid command. Type 'help' for a list of available commands.\n");
        }





    }
    return 0;
}
