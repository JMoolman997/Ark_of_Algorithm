#ifndef BOOLEAN_H
#define BOOLEAN_H

/**
 * @brief A Boolean type definition.
 */
typedef enum boolean {
	FALSE, /**< symbol for Boolean false */
	TRUE   /**< symbol for Boolean true  */
} Boolean;

/** a macro to return a string value from a boolean; useful for debugging. */
#define BOOLEAN_VALUE(x) ((x) ? "TRUE" : "FALSE")

#endif /* BOOLEAN_H */

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

typedef enum {
    VOGELS_APPROXIMATION,
    NORTH_WEST_CORNER,
    LEAST_COST
} AllocationMethod;

// Input functions
void parse_comma_separated_values(const char *input, int *array, int expected_size);
int get_confirmation(const char *message);
void input_vector(int *vector, int size, const char *prompt);
void input_matrix(int **matrix, int rows, int cols, const char *prompt);

// Utility functions
void print_vector(const int *vector, int size);
void print_matrix(int **matrix, int rows, int cols);

// Allocation methods
int vogels_approximation_method(TransportProblem *tp, int **results, Boolean print_iterations);
int north_west_corner_method(TransportProblem *tp, int **results, Boolean print_iterations);
int least_cost_method(TransportProblem *tp, int **results, Boolean print_iterations);

#endif // TRANSPORT_H
