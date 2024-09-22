/**
 * @file network.c
 * @author bpentek
 * @brief C source file of EDR random network model generation
 * @version 0.1
 * @date 2024-09-22
 * 
 * 
 */
#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

/**
 * @brief Function that performs binary search in bins of distance histogram. Used in EDR model network generation.
 * 
 * @param bins Array of bins
 * @param nr_bins Total number of bins
 * @param bin_width Width of a bin
 * @param value Searched value
 * @return int `i` index of bin for which: bins[i] < value <= bins[i] + bin_width (returns -1 if not found)
 */
int binary_search(double * bins, size_t nr_bins, double bin_width, double value)
{
    size_t lower = 0, upper = nr_bins - 1, middle;
    double lower_bin, upper_bin;

    while (lower <= upper)
    {
        middle = lower + (upper - lower)/2;
        lower_bin = bins[middle];
        upper_bin = lower_bin + bin_width;
        
        if (lower_bin < value && value <= upper_bin)
        {
            return middle; // found
        }

        if (value <= lower_bin)
        {
            upper = middle - 1;
        }
        else
        {
            lower = middle + 1;
        }
    }

    return -1; // not found
}

/**
 * @brief Function that generates and exports a CDR random network model (based on algorithm developed by Ercsey-Ravasz et al. 2013)
 * 
 * @param output_path Path to output edge list of newly generated CDR random network.
 * @param nr_nodes Number of nodes
 * @param nr_edges Number of edges
 * @param nr_bins Number of (distance) bins
 * @param dist_bins Array of distance bins
 * @param dist_hist Array of distance histogram values
 * @param dist_indices Array of distance bin node pair indices
 * @param rng Random number generator from the GSL library
 * @return int Success/error code
 */
int generate_CDR_network(char * output_path, size_t nr_nodes, size_t nr_edges, size_t nr_bins, double * dist_bins, int * dist_hist, int ** dist_indices, gsl_rng * rng)
{
    // INITIALIZING VARIABLES
    double dist; // random distance
    double bin_width = dist_bins[1] - dist_bins[0]; // width of a bin
    double max_dist_bin = dist_bins[nr_bins]; // max distance
    size_t bin_idx, edge_idx; // indices of bin & edge
    size_t direction, source, target, tmp; // direction; source and target nodes of edge to be inserted
    size_t count_edges = 0; // count of total edges in the model network
    int ** weights = malloc(nr_nodes*sizeof(int *)); // weighted adjacency matrix of random network
    if (weights == NULL) { printf("ERROR: Failed to allocate memory for weight matrix."); return 1; }
    for (size_t i = 0; i < nr_nodes; i++)
    {
        weights[i] = malloc(nr_nodes*sizeof(int));
        if (weights[i] == NULL) { printf("ERROR: Failed to allocate memory for weight matrix."); return 1; }
        for (size_t j = 0; j < nr_nodes; j++)
        {
            weights[i][j] = 0;
        }
    }
    
    // GENERATING RANDOM NETWORK
    do
    {
        // 1. DRAW RANDOMLY A DISTANCE FROM CONSTANT DISTANCE RULE
        dist = gsl_rng_uniform(rng)*max_dist_bin;
        // 2. CHOOSE UNIFORMLY AT RANDOM NODE PAIR (source, target) FOR WHICH: dist < D(source, target) <= dist + bin_width
        bin_idx = binary_search(dist_bins, nr_bins + 1, bin_width, dist);
        if (bin_idx == -1 || dist_hist[bin_idx] == 0) { continue; } // skip 
        edge_idx = gsl_rng_uniform_int(rng, dist_hist[bin_idx]);
        // 3. INSERTING RANDOMLY DIRECTED EDGE BETWEEN NODES (source, target)
        direction = gsl_rng_uniform_int(rng, 2);
        source = dist_indices[bin_idx][2*edge_idx];
        target = dist_indices[bin_idx][2*edge_idx + 1]; // NOTE: source_1 target_1 source_2 target_2 ...
        if (direction == 1){ tmp = source; source = target; target = tmp;} // flip edge direction
        if (weights[source][target] == 0.0) { count_edges++; } // increase count of inserted edges
        weights[source][target]++; // increase weight
    
    } while(count_edges < nr_edges); // 4. REPEAT STEPS UNTIL NUMBER OF INSERTED EDGES REACHES TARGET 
    
    // EXPORTING NETWORK EDGE LIST (FORMAT: source target weight)
    FILE * fp = fopen(output_path, "w");
    if (fp == NULL) { printf("ERROR: Could not open output file '%s'", output_path); return 1; }
    for (size_t i = 0; i < nr_nodes; i++)
    {
        for (size_t j = 0; j< nr_nodes; j++)
        {
            if (weights[i][j] != 0.0)
            {
                fprintf(fp, "%ld %ld %d\n", i, j, weights[i][j]);
            }
        }
    }
    
    // cleaning up
    fclose(fp);
    for (size_t i = 0; i < nr_nodes; i++) { free(weights[i]); }
    free(weights);

    return 0;
}

/**
 * @brief Function that generates and exports a EDR random network model (based on algorithm developed by Ercsey-Ravasz et al. 2013)
 * 
 * @param output_path Path to output edge list of newly generated EDR random network.
 * @param nr_nodes Number of nodes
 * @param nr_edges Number of edges
 * @param nr_bins Number of (distance) bins
 * @param dist_bins Array of distance bins
 * @param dist_hist Array of distance histogram values
 * @param dist_indices Array of distance bin node pair indices
 * @param rng Random number generator from the GSL library
 * @param lambda Lambda decay parameter of the exponential distribution/distance rule (p(d) ~ exp(-lambda*d))
 * @return int Success/error code
 */
int generate_EDR_network(char * output_path, size_t nr_nodes, size_t nr_edges, size_t nr_bins, double * dist_bins, int * dist_hist, int ** dist_indices, gsl_rng * rng, double lambda)
{
    printf("LAMBDA: %lf\n", lambda);
    // INITIALIZING VARIABLES
    double dist; // random distance
    double bin_width = dist_bins[1] - dist_bins[0]; // width of a bin
    size_t bin_idx, edge_idx; // indices of bin & edge
    size_t direction, source, target, tmp; // direction; source and target nodes of edge to be inserted
    size_t count_edges = 0; // count of total edges in the model network
    int ** weights = malloc(nr_nodes*sizeof(int *)); // weighted adjacency matrix of random network
    if (weights == NULL) { printf("ERROR: Failed to allocate memory for weight matrix."); return 1; }
    for (size_t i = 0; i < nr_nodes; i++)
    {
        weights[i] = malloc(nr_nodes*sizeof(int));
        if (weights[i] == NULL) { printf("ERROR: Failed to allocate memory for weight matrix."); return 1; }
        for (size_t j = 0; j < nr_nodes; j++)
        {
            weights[i][j] = 0;
        }
    }
    // GENERATING RANDOM NETWORK
    do
    {
        // 1. DRAW RANDOMLY A DISTANCE FROM CONSTANT DISTANCE RULE
        dist = gsl_ran_exponential(rng, 1.0/lambda);
        // 2. CHOOSE UNIFORMLY AT RANDOM NODE PAIR (source, target) FOR WHICH: dist < D(source, target) <= dist + bin_width
        bin_idx = binary_search(dist_bins, nr_bins + 1, bin_width, dist);
        if (bin_idx == -1 || dist_hist[bin_idx] == 0) { continue; } // skip 
        edge_idx = gsl_rng_uniform_int(rng, dist_hist[bin_idx]); // select node pair (edge index) randomly
        
        // 3. INSERTING RANDOMLY DIRECTED EDGE BETWEEN NODES (source, target)
        direction = gsl_rng_uniform_int(rng, 2);
        source = dist_indices[bin_idx][2*edge_idx];
        target = dist_indices[bin_idx][2*edge_idx + 1]; // NOTE: source_1 target_1 source_2 target_2 format
        if (direction == 1){ tmp = source; source = target; target = tmp;} // flip edge direction
        if (weights[source][target] == 0.0) { count_edges++; } // increase count of inserted edges
        weights[source][target]++; // increase weight
    
    } while(count_edges < nr_edges); // 4. REPEAT STEPS UNTIL NUMBER OF INSERTED EDGES REACHES TARGET 

    // EXPORTING NETWORK EDGE LIST (FORMAT: source target weight)
    FILE * fp = fopen(output_path, "w");
    if (fp == NULL) { printf("ERROR: Could not open output file '%s'", output_path); return 1; }
    for (size_t i = 0; i < nr_nodes; i++)
    {
        for (size_t j = 0; j< nr_nodes; j++)
        {
            if (weights[i][j] != 0.0)
            {
                fprintf(fp, "%ld %ld %d\n", i, j, weights[i][j]);
            }
        }
    }
    
    // cleaning up
    fclose(fp);
    for (size_t i = 0; i < nr_nodes; i++) { free(weights[i]); }
    free(weights);

    return 0;
}