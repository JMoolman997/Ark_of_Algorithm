#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transport.h"

// Function to split a string by commas and convert to integers
void parse_comma_separated_values(
        const char *input,
        int *array,
        int expected_size
) {
    char *token;
    char input_copy[MAX_INPUT_SIZE];
    int index = 0;

    strcpy(input_copy, input);  // Copy since strtok modifies the string
    token = strtok(input_copy, ",");

    while (token != NULL && index < expected_size) {
        array[index] = atoi(token);
        token = strtok(NULL, ",");
        index++;
    }

    if (index != expected_size) {
        printf("Warning: Expected %d values, but got %d.\n", expected_size, index);
    }
}

// Function to get confirmation from the user
int get_confirmation(
        const char *message
) {
    char response[MAX_INPUT_SIZE];
    printf("%s (y/n): ", message);
    scanf("%s", response);
    return (strcmp(response, "y") == 0 || strcmp(response, "Y") == 0);
}

// Function to input a vector with comma-separated values
void input_vector(
        int *vector,
        int size,
        const char *prompt
) {
    int valid = 0;

    while (!valid) {
        char input[MAX_INPUT_SIZE];
        printf("%s (comma-separated, size %d): ", prompt, size);
        scanf(" %[^\n]", input);  // Read the entire line
        
        parse_comma_separated_values(input, vector, size);

        printf("Vector entered: ");
        print_vector(vector, size);
        if (get_confirmation("Is this correct?")) {
            valid = 1;
        }
    }
}

// Function to input a matrix
void input_matrix(
        int **matrix,
        int rows,
        int cols,
        const char *prompt
) {
    for (int i = 0; i < rows; i++) {
        int valid = 0;
        while (!valid) {
            char input[MAX_INPUT_SIZE];
            printf("%s (row %d, comma-separated, size %d): ", prompt, i + 1, cols);
            scanf(" %[^\n]", input);
            
            parse_comma_separated_values(input, matrix[i], cols);

            printf("Row %d entered: ", i + 1);
            print_vector(matrix[i], cols);
            if (get_confirmation("Is this correct?")) {
                valid = 1;
            }
        }
    }
}

// Utility function to print a vector
void print_vector(
        const int *vector,
        int size
) {
    for (int i = 0; i < size; i++) {
        printf("%d ", vector[i]);
    }
    printf("\n");
}

// Utility function to print a matrix
void print_matrix(
        int **matrix,
        int rows,
        int cols
) {
    for (int i = 0; i < rows; i++) {
        print_vector(matrix[i], cols);
    }
}
