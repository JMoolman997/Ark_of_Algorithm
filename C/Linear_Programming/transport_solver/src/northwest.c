#include <stdio.h>
#include <stdlib.h>
#include "transport.h"

/**
 * Solves the transportation problem using the North-West Corner Method.
 * 
 * @param tp Pointer to the TransportationProblem structure containing supply, demand, and cost matrix.
 * @param results Pre-allocated 2D array to store the allocation results.
 * @param print_iterations Boolean flag to enable/disable printing each allocation step.
 * @return Total cost of the transportation plan.
 */
int north_west_corner_method(
        TransportProblem *tp,
        int **results,
        Boolean print_iterations
) {
    int i = 0; // Row index
    int j = 0; // Column index
    int total_cost = 0;
    int allocation;
    int iteration = 0;

    // Create copies of supply and demand to avoid modifying the original data
    int *supply = (int *)malloc(tp->supply_size * sizeof(int));
    int *demand = (int *)malloc(tp->demand_size * sizeof(int));

    if (!supply || !demand) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int k = 0; k < tp->supply_size; k++) {
        supply[k] = tp->supply[k];
    }

    for (int k = 0; k < tp->demand_size; k++) {
        demand[k] = tp->demand[k];
    }

    // Initialize results matrix to zeros
    for (int r = 0; r < tp->supply_size; r++) {
        for (int c = 0; c < tp->demand_size; c++) {
            results[r][c] = 0;
        }
    }

    // Allocation loop
    while (i < tp->supply_size && j < tp->demand_size) {
        allocation = (supply[i] < demand[j]) ? supply[i] : demand[j];
        results[i][j] = allocation;
        total_cost += allocation * tp->cost_matrix[i][j];
        iteration++;

        if (print_iterations) {
            printf("Allocation %d: %d units to cell (%d, %d) with cost %d.\n",
                   iteration, allocation, i, j, tp->cost_matrix[i][j]);
            printf("  Remaining Supply[%d]: %d\n", i, supply[i] - allocation);
            printf("  Remaining Demand[%d]: %d\n\n", j, demand[j] - allocation);
        }

        supply[i] -= allocation;
        demand[j] -= allocation;

        if (supply[i] == 0 && i < tp->supply_size - 1) {
            i++;
        } else if (demand[j] == 0 && j < tp->demand_size - 1) {
            j++;
        }
    }

    // Free allocated memory
    free(supply);
    free(demand);

    return total_cost;
}
