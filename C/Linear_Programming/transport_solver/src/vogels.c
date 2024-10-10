#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "transport.h"

#define MAX_ITERATIONS 1000 // To prevent potential infinite loops

/**
 * Computes the difference between the two smallest costs in a row or column.
 * 
 * @param index Index of the row or column.
 * @param len Length of the row or column.
 * @param is_row Boolean indicating whether to process a row (TRUE) or column (FALSE).
 * @param res Array to store the results: res[0] = min2 - min1, res[1] = min1, res[2] = index of min1.
 * @param tp Pointer to the TransportationProblem structure.
 * @param row_done Array indicating completed rows.
 * @param col_done Array indicating completed columns.
 */
static void diff(
        int index,
        int len,
        Boolean is_row,
        int *res,
        TransportProblem *tp,
        Boolean *row_done,
        Boolean *col_done
) {
    int i, c;
    int min1 = INT_MAX, min2 = INT_MAX, min_p = -1;

    for (i = 0; i < len; ++i) {
        if (is_row) {
            if (col_done[i]) continue;
            c = tp->cost_matrix[index][i];
        } else {
            if (row_done[i]) continue;
            c = tp->cost_matrix[i][index];
        }
        if (c < min1) {
            min2 = min1;
            min1 = c;
            min_p = i;
        } else if (c < min2) {
            min2 = c;
        }
    }

    res[0] = (min2 != INT_MAX) ? (min2 - min1) : 0; // Handle cases with less than two costs
    res[1] = min1;
    res[2] = min_p;
}

/**
 * Finds the row or column with the maximum penalty.
 * 
 * @param len1 Number of rows or columns.
 * @param len2 Number of columns or rows.
 * @param is_row Boolean indicating whether to process rows (TRUE) or columns (FALSE).
 * @param res Array to store the results:
 *            res[0] = row or column index with maximum penalty,
 *            res[1] = position of minimum cost within that row or column,
 *            res[2] = minimum cost,
 *            res[3] = maximum penalty value.
 * @param tp Pointer to the TransportationProblem structure.
 * @param row_done Array indicating completed rows.
 * @param col_done Array indicating completed columns.
 */
static void max_penalty(
        int len1,
        int len2,
        Boolean is_row,
        int *res,
        TransportProblem *tp,
        Boolean *row_done,
        Boolean *col_done
) {
    int i;
    int pc = -1, pm = -1, mc = -1;
    int md = INT_MIN; // Maximum difference (penalty)
    int res2[3];

    for (i = 0; i < len1; ++i) {
        if ((is_row && row_done[i]) || (!is_row && col_done[i])) continue;
        diff(i, len2, is_row, res2, tp, row_done, col_done);
        if (res2[0] > md) {
            md = res2[0];  // Update maximum difference
            pm = i;        // Index of row or column with maximum penalty
            mc = res2[1];  // Minimum cost in that row or column
            pc = res2[2];  // Position of minimum cost
        }
    }

    if (is_row) {
        res[0] = pm;
        res[1] = pc;
    } else {
        res[0] = pc;
        res[1] = pm;
    }
    res[2] = mc;
    res[3] = md;
}

/**
 * Determines the next cell to allocate based on the maximum penalty.
 * 
 * @param res Array to store the results: res[0] = row index, res[1] = column index.
 * @param tp Pointer to the TransportationProblem structure.
 * @param row_done Array indicating completed rows.
 * @param col_done Array indicating completed columns.
 */
static void next_cell(
        int *res,
        TransportProblem *tp,
        Boolean *row_done,
        Boolean *col_done
) {
    int res1[4], res2[4];
    int i;

    // Find the row with the maximum penalty
    max_penalty(tp->supply_size, tp->demand_size, TRUE, res1, tp, row_done, col_done);
    // Find the column with the maximum penalty
    max_penalty(tp->demand_size, tp->supply_size, FALSE, res2, tp, row_done, col_done);

    // Compare penalties and choose the one with the higher penalty
    if (res1[3] == res2[3]) {
        // If penalties are equal, choose the one with the lower cost
        if (res1[2] < res2[2]) {
            for (i = 0; i < 4; ++i) res[i] = res1[i];
        } else {
            for (i = 0; i < 4; ++i) res[i] = res2[i];
        }
    } else if (res1[3] > res2[3]) {
        for (i = 0; i < 4; ++i) res[i] = res2[i];
    } else {
        for (i = 0; i < 4; ++i) res[i] = res1[i];
    }
}

/**
 * Solves the transportation problem using Vogel's Approximation Method.
 * 
 * @param tp Pointer to the TransportationProblem structure containing supply, demand, and cost matrix.
 * @param results Pre-allocated 2D array to store the allocation results.
 * @param print_iterations Boolean flag to enable/disable printing each iteration.
 * @return Total cost of the transportation plan.
 */
int vogels_approximation_method(
        TransportProblem *tp,
        int **results,
        Boolean print_iterations
) {
    int i, j;
    int *supply = (int *)malloc(tp->supply_size * sizeof(int));
    int *demand = (int *)malloc(tp->demand_size * sizeof(int));
    Boolean *row_done = (Boolean *)calloc(tp->supply_size, sizeof(Boolean));
    Boolean *col_done = (Boolean *)calloc(tp->demand_size, sizeof(Boolean));
    int total_cost = 0;
    int supply_left = 0;
    int cell[4];
    int iteration = 0;

    // Check for memory allocation success
    if (!supply || !demand || !row_done || !col_done) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    // Copy supply and demand to local arrays to avoid modifying the original data
    for (i = 0; i < tp->supply_size; i++) {
        supply[i] = tp->supply[i];
        supply_left += supply[i];
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

    // Main loop to compute the transportation plan
    while (supply_left > 0 && iteration < MAX_ITERATIONS) {
        iteration++;
        // Determine the next cell to allocate
        next_cell(cell, tp, row_done, col_done);
        int r = cell[0]; // Row index
        int c = cell[1]; // Column index

        // Determine the allocation quantity
        int q = (demand[c] <= supply[r]) ? demand[c] : supply[r];

        // Update demand and supply
        demand[c] -= q;
        if (demand[c] == 0) col_done[c] = TRUE;
        supply[r] -= q;
        if (supply[r] == 0) row_done[r] = TRUE;

        // Store the allocation in the results matrix
        results[r][c] = q;

        // Update the remaining supply
        supply_left -= q;

        // Update total cost
        total_cost += q * tp->cost_matrix[r][c];

        // Print iteration details if enabled
        if (print_iterations) {
            printf("Iteration %d:\n", iteration);
            printf("  Allocated %d units to cell (%d, %d) with cost %d.\n", q, r, c, tp->cost_matrix[r][c]);
            printf("  Remaining Supply: ");
            for (i = 0; i < tp->supply_size; i++) {
                printf("%d ", supply[i]);
            }
            printf("\n  Remaining Demand: ");
            for (i = 0; i < tp->demand_size; i++) {
                printf("%d ", demand[i]);
            }
            printf("\n\n");
        }
    }

    if (iteration == MAX_ITERATIONS) {
        fprintf(stderr, "Maximum iterations reached. Possible issue with the transportation problem.\n");
    }

    // Free allocated memory
    free(supply);
    free(demand);
    free(row_done);
    free(col_done);

    return total_cost;
}
