#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"

int main() {

    struct node* root = (struct node*)createLeafNode(0,0);
    int intialLeafNumOfKeys = 0;

    char input[100];
    while (1)
    {

        root = getRoot(root);

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
            printTree(root);
        }
        else if (strcmp(input, "insert") == 0)
        {
            int key;
            printf("Enter key to insert into the tree: ");
            scanf("%d", &key);
            getchar(); // consume newline character left in the input buffer by scanf     

            insertIntoTree(root, key,root);

        }
        else if (strcmp(input, "delete") == 0)
        {
            printf("deleting key\n");
        }
        else if (strcmp(input, "search") == 0)
        {
            int key;
            printf("Enter key to search: ");
            scanf("%d", &key);
            getchar(); // consume newline character left in the input buffer by scanf
            search(key,root);

        }
        else if (strcmp(input, "asctest") == 0)
        {
            for (int j = 1; j <= 1000000; j++)
            {
                root = getRoot(root);
                insertIntoTree(root,j,root);
            }
        }else if (strcmp(input, "desctest") == 0)
        {
            for (int i = 1000000; i >= 1; i--)
            {
                root = getRoot(root);
                insertIntoTree(root,i,root);
            }
        }
        else if (strcmp(input, "searchtest") == 0)
        {
            for (int i = 100; i >= 1; i--)
            {
                root = getRoot(root);
                search(i,root);
            }
        }
        else
        {
            printf("Invalid command. Type 'help' for a list of available commands.\n");
        }

    }
    return 0;
}
