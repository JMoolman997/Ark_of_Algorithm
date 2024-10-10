#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "transport.h"

/**
 * Finds the cell with the minimum cost that is not yet allocated.
 * 
 * @param tp Pointer to the TransportationProblem structure.
 * @param row_done Array indicating completed rows.
 * @param col_done Array indicating completed columns.
 * @param min_row Pointer to store the row index of the minimum cost cell.
 * @param min_col Pointer to store the column index of the minimum cost cell.
 * @return TRUE if a minimum cost cell is found, FALSE otherwise.
 */
static Boolean find_min_cost_cell(
    TransportProblem *tp,
    Boolean *row_done,
    Boolean *col_done,
    int *min_row,
    int *min_col
) {
    int min_cost = INT_MAX;
    int r = -1, c = -1;

    for (int i = 0; i < tp->supply_size; i++) {
        if (row_done[i]) continue;
        for (int j = 0; j < tp->demand_size; j++) {
            if (col_done[j]) continue;
            if (tp->cost_matrix[i][j] < min_cost) {
                min_cost = tp->cost_matrix[i][j];
                r = i;
                c = j;
            }
        }
    }

    if (r != -1 && c != -1) {
        *min_row = r;
        *min_col = c;
        return TRUE;
    }

    return FALSE;
}

/**
 * Solves the transportation problem using the Least Cost Cell Method.
 * 
 * @param tp Pointer to the TransportationProblem structure containing supply, demand, and cost matrix.
 * @param results Pre-allocated 2D array to store the allocation results.
 * @param print_iterations Boolean flag to enable/disable printing each allocation step.
 * @return Total cost of the transportation plan.
 */
int least_cost_method(
    TransportProblem *tp,
    int **results,
    Boolean print_iterations
) {
    int total_cost = 0;
    int iteration = 0;
    int i, j, allocation;

    // Create copies of supply and demand to avoid modifying the original data
    int *supply = (int *)malloc(tp->supply_size * sizeof(int));
    int *demand = (int *)malloc(tp->demand_size * sizeof(int));
    Boolean *row_done = (Boolean *)calloc(tp->supply_size, sizeof(Boolean));
    Boolean *col_done = (Boolean *)calloc(tp->demand_size, sizeof(Boolean));

    if (!supply || !demand || !row_done || !col_done) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < tp->supply_size; i++) {
        supply[i] = tp->supply[i];
    }

    for (j = 0; j < tp->demand_size; j++) {
        demand[j] = tp->demand[j];
    }

    // Initialize results matrix to zeros
    for (i = 0; i < tp->supply_size; i++) {
        for (j = 0; j < tp->demand_size; j++) {
            results[i][j] = 0;
        }
    }

    // Allocation loop
    while (1) {
        int min_row, min_col;
        if (!find_min_cost_cell(tp, row_done, col_done, &min_row, &min_col)) {
            break;  // No more cells to allocate
        }

        // Determine the allocation quantity
        allocation = (supply[min_row] < demand[min_col]) ? supply[min_row] : demand[min_col];
        results[min_row][min_col] = allocation;
        total_cost += allocation * tp->cost_matrix[min_row][min_col];
        iteration++;

        if (print_iterations) {
            printf("Allocation %d: %d units to cell (%d, %d) with cost %d.\n",
                   iteration, allocation, min_row, min_col, tp->cost_matrix[min_row][min_col]);
            printf("  Remaining Supply[%d]: %d\n", min_row, supply[min_row] - allocation);
            printf("  Remaining Demand[%d]: %d\n\n", min_col, demand[min_col] - allocation);
        }

        // Update supply and demand
        supply[min_row] -= allocation;
        demand[min_col] -= allocation;

        // Mark row or column as done
        if (supply[min_row] == 0) {
            row_done[min_row] = TRUE;
        }

        if (demand[min_col] == 0) {
            col_done[min_col] = TRUE;
        }
    }

    // Free allocated memory
    free(supply);
    free(demand);
    free(row_done);
    free(col_done);

    return total_cost;
}
