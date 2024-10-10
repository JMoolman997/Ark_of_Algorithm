#ifndef TRANSPORT_H
#define TRANSPORT_H

#define MAX_INPUT_SIZE 1000

typedef struct {
    int supply_size;
    int demand_size;
    int *supply;
    int *demand;
    int **cost_matrix;
} TransportProblem;

// Function prototypes

// Input functions
void parse_comma_separated_values(const char *input, int *array, int expected_size);
int get_confirmation(const char *message);
void input_vector(int *vector, int size, const char *prompt);
void input_matrix(int **matrix, int rows, int cols, const char *prompt);

// Utility functions
void print_vector(const int *vector, int size);
void print_matrix(int **matrix, int rows, int cols);

#endif // TRANSPORT_H
