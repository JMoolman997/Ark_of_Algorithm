#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transport.h"

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

    // Free allocated memory
    free(tp.supply);
    free(tp.demand);
    for (int i = 0; i < tp.supply_size; i++) {
        free(tp.cost_matrix[i]);
    }
    free(tp.cost_matrix);

    return 0;
}
