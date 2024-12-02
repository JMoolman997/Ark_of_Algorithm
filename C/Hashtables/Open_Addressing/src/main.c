#include <stdio.h>
#include <stdlib.h>
#include "open_addressing.h"

void keyval2str(int flag, KEY_TYPE k, VALUE_TYPE v, char *buffer) {
    if (flag == 1) {
        sprintf(buffer, "Key: %d, Value: %d", (int)k, (int)v);
    } else if (flag == 2) {
        sprintf(buffer, "Deleted");
    } else {
        sprintf(buffer, "Empty");
    }
}

int main(void) {
    HashTab *ht = init_ht(0, 0, 0, 0, 0, NULL, NULL, LINEAR);

    if (ht == NULL) {
        fprintf(stderr, "Failed to initialize hash table.\n");
        return EXIT_FAILURE;
    }

    int choice, key, value, index;
    while (1) {
        printf("\nHash Table Menu:\n");
        printf("1. Insert Key-Value Pair\n");
        printf("2. Search for Key\n");
        printf("3. Remove Key\n");
        printf("4. Print Hash Table\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: // Insert
                printf("Enter key: ");
                scanf("%d", &key);
                printf("Enter value: ");
                scanf("%d", &value);
                if (insert_ht(ht, key, value) == HT_SUCCESS) {
                    printf("Key-Value pair inserted successfully.\n");
                } else {
                    printf("Error inserting Key-Value pair.\n");
                }
                break;
            case 2: // Search
                printf("Enter key to search: ");
                scanf("%d", &key);
                index = search_ht(ht, key);
                if (index >= 0) {
                    printf("Key %d found with value: %d\n", key, fetch_ht(ht, index));
                } else {
                    printf("Key %d not found.\n", key);
                }
                break;
            case 3: // Remove
                printf("Enter key to remove: ");
                scanf("%d", &key);
                if (remove_ht(ht, key) == HT_SUCCESS) {
                    printf("Key %d removed successfully.\n", key);
                } else {
                    printf("Key %d not found.\n", key);
                }
                break;
            case 4: // Print
                print_ht(ht, keyval2str);
                break;
            case 5: // Exit
                free_ht(ht, NULL, NULL);
                printf("Exiting...\n");
                return EXIT_SUCCESS;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}
