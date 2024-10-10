#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transport.h"

/**
 * Function to print the allocation matrix.
 * 
 * @param matrix 2D array containing the allocation results.
 * @param rows Number of supply points.
 * @param cols Number of demand points.
 */
void print_allocation_matrix(
        int **matrix,
        int rows,
        int cols
) {
    int i, j;

    printf("\nTransportation Plan (Allocation Matrix):\n");
    printf("    ");
    for (j = 0; j < cols; j++) {
        printf("D%-3d", j + 1);
    }
    printf("\n");

    for (i = 0; i < rows; i++) {
        printf("S%-3d", i + 1);
        for (j = 0; j < cols; j++) {
            printf("%-4d", matrix[i][j]);
        }
        printf("\n");
    }
}

/**
 * Balances the transportation problem by adding dummy supply or demand nodes if necessary.
 * 
 * @param tp Pointer to the TransportationProblem structure to be balanced.
 */
void balance_transport_problem(TransportProblem *tp) {
    int total_supply = 0, total_demand = 0;

    // Calculate total supply
    for (int i = 0; i < tp->supply_size; i++) {
        total_supply += tp->supply[i];
    }

    // Calculate total demand
    for (int j = 0; j < tp->demand_size; j++) {
        total_demand += tp->demand[j];
    }

    // Supply > Demand: Add Dummy Demand
    if (total_supply > total_demand) {
        int dummy_demand = total_supply - total_demand;
        tp->demand = realloc(tp->demand, (tp->demand_size + 1) * sizeof(int));
        if (!tp->demand) {
            fprintf(stderr, "Memory allocation failed for dummy demand.\n");
            exit(EXIT_FAILURE);
        }
        tp->demand[tp->demand_size] = dummy_demand;
        tp->demand_size++;

        // Add zero-cost column to the cost matrix
        for (int i = 0; i < tp->supply_size; i++) {
            tp->cost_matrix[i] = realloc(tp->cost_matrix[i], tp->demand_size * sizeof(int));
            if (!tp->cost_matrix[i]) {
                fprintf(stderr, "Memory allocation failed for cost matrix expansion.\n");
                exit(EXIT_FAILURE);
            }
            tp->cost_matrix[i][tp->demand_size - 1] = 0;  // Zero cost for dummy demand
        }
    }
    // Demand > Supply: Add Dummy Supply
    else if (total_demand > total_supply) {
        int dummy_supply = total_demand - total_supply;
        tp->supply = realloc(tp->supply, (tp->supply_size + 1) * sizeof(int));
        if (!tp->supply) {
            fprintf(stderr, "Memory allocation failed for dummy supply.\n");
            exit(EXIT_FAILURE);
        }
        tp->supply[tp->supply_size] = dummy_supply;
        tp->supply_size++;

        // Add zero-cost row to the cost matrix
        tp->cost_matrix = realloc(tp->cost_matrix, tp->supply_size * sizeof(int *));
        if (!tp->cost_matrix) {
            fprintf(stderr, "Memory allocation failed for cost matrix row addition.\n");
            exit(EXIT_FAILURE);
        }
        tp->cost_matrix[tp->supply_size - 1] = (int *)malloc(tp->demand_size * sizeof(int));
        if (!tp->cost_matrix[tp->supply_size - 1]) {
            fprintf(stderr, "Memory allocation failed for dummy supply row.\n");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < tp->demand_size; j++) {
            tp->cost_matrix[tp->supply_size - 1][j] = 0;  // Zero cost for dummy supply
        }
    }
    // Balanced: No Action Needed
    else {
        // The problem is already balanced
        return;
    }
}

int main() {

    char choice[MAX_INPUT_SIZE];
    int supply_size, demand_size;
    TransportProblem tp;

    // Ask if the user wants to solve a transportation problem
    printf("Do you want to solve a transportation problem? (y/n): ");
    scanf("%s", choice);

    if (strcmp(choice, "y") != 0 && strcmp(choice, "Y") != 0) {
        printf("Exiting...\n");
        return 0;
    }

    // Input size for supply and demand
    printf("Enter the number of supply points: ");
    scanf("%d", &supply_size);
    printf("Enter the number of demand points: ");
    scanf("%d", &demand_size);

    // Initialize the transportation problem
    tp.supply_size = supply_size;
    tp.demand_size = demand_size;

    // Allocate memory for supply and demand vectors
    tp.supply = (int *)malloc(supply_size * sizeof(int));
    tp.demand = (int *)malloc(demand_size * sizeof(int));

    // Input supply vector
    input_vector(tp.supply, supply_size, "Enter the supply vector");

    // Input demand vector
    input_vector(tp.demand, demand_size, "Enter the demand vector");

    // Allocate memory for cost matrix
    tp.cost_matrix = (int **)malloc(supply_size * sizeof(int *));
    for (int i = 0; i < supply_size; i++) {
        tp.cost_matrix[i] = (int *)malloc(demand_size * sizeof(int));
    }

    // Input cost matrix
    input_matrix(tp.cost_matrix, supply_size, demand_size, "Enter the cost matrix");

    // Output final result for confirmation
    printf("\nFinal Supply Vector: ");
    print_vector(tp.supply, tp.supply_size);
    printf("Final Demand Vector: ");
    print_vector(tp.demand, tp.demand_size);
    printf("Final Cost Matrix: \n");
    print_matrix(tp.cost_matrix, tp.supply_size, tp.demand_size);

    // Balance the transportation problem
    balance_transport_problem(&tp);

    // Allocate results matrix
    int **results = (int **)malloc(tp.supply_size * sizeof(int *));
    for (int i = 0; i < tp.supply_size; i++) {
        results[i] = (int *)malloc(tp.demand_size * sizeof(int));
        if (!results[i]) {
            fprintf(stderr, "Memory allocation failed for results row %d.\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Ask user for solution method
    printf("Select Allocation Method:\n");
    printf("1. Vogel's Approximation Method\n");
    printf("2. North-West Corner Method\n");
    printf("3. Least Cost Method\n");  // Added Least Cost Method option

    int selected;
    while (1) {
        printf("Enter your choice(1,2,3): ");
        if (scanf("%d", &selected) == 1) {
            break; // Valid input received
        } else {
            fprintf(stderr, "Invalid input. Please enter an integer.\n");
            // Clear the input buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
        }
    }

    AllocationMethod method;
    switch (selected) {
        case 1:
            method = VOGELS_APPROXIMATION;
            break;
        case 2:
            method = NORTH_WEST_CORNER;
            break;
        case 3:
            method = LEAST_COST;
            break;
        default:
            fprintf(stderr, "Invalid choice.\n");
            exit(EXIT_FAILURE);
    }

    // Ask user if they want to print each iteration
    printf("Do you want to print each iteration? (y/n): ");
    Boolean print_iterations = FALSE;
    scanf("%s", choice);
    if (strcmp(choice, "y") == 0 || strcmp(choice, "Y") == 0) {
        print_iterations = TRUE;
    }

    int total_cost = 0;
    switch (method) {
        case VOGELS_APPROXIMATION:
            total_cost = vogels_approximation_method(&tp, results, print_iterations);
            break;
        case NORTH_WEST_CORNER:
            total_cost = north_west_corner_method(&tp, results, print_iterations);
            break;
        case LEAST_COST:
            total_cost = least_cost_method(&tp, results, print_iterations);
            break;
        // Add more cases for additional methods
        default:
            fprintf(stderr, "Method not implemented.\n");
            exit(EXIT_FAILURE);
    }

    printf("\nSolution:\n");
    print_matrix(results, tp.supply_size, tp.demand_size);
    printf("Total Cost: %d\n", total_cost);

    // Free allocated memory
    for (int i = 0; i < tp.supply_size; i++) {
        free(tp.cost_matrix[i]);
        free(results[i]);
    }
    free(tp.cost_matrix);
    free(tp.supply);
    free(tp.demand);
    free(results);

    return 0;
}

