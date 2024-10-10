#ifndef VOGELS_H
#define VOGELS_H

#include "transport.h"

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
);

#endif // VOGELS_H
