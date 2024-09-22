/**
 * @file generate_network.c
 * @author bpentek
 * @brief C program that generates EDR random network model.
 * @version 0.1
 * @date 2024-09-22
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "distance.h"
#include "network.h"

int check_input_args(size_t NR_NODES, size_t NR_EDGES, char * DIST_BINS_PATH, char * DIST_HIST_PATH, char * DIST_IDX_PATH, double LAMBDA);

int main(int argc, char * argv[])
{
    if (argc < 8)
    {
        printf("USAGE: %s <NUMBER OF NODES (int)> <NUMBER OF EDGES (int)> </PATH/TO/DISTANCE/HISTOGRAM/BINS> </PATH/TO/DISTANCE/HISTOGRAM/VALUES> </PATH/TO/DISTANCE/HISTOGRAM/INDICES> <LAMBDA DECAY PARAMETER (double)> </PATH/TO/OUTPUT/EDGELIST> <RANDOM NUMBER GENERATOR SEED [optional]>", argv[0]);
        return 1;
    }
    // input arguments
    size_t NR_NODES = atoi(argv[1]); // number of nodes
    size_t NR_EDGES = atoi(argv[2]); // number of edges
    char * DIST_BINS_PATH = argv[3]; // path to bins of distance histogram (resulting from distance matrix of brain regions)
    char * DIST_HIST_PATH = argv[4]; // path to values of distance histogram (counts)
    char * DIST_IDX_PATH = argv[5]; // path to indices in bins of distance histogram (node pairs)
    double LAMBDA = strtof(argv[6], NULL); // lambda decay parameter of exponential distribution
    char * OUTPUT_PATH = argv[7]; // path to output edge list
    unsigned int seed;
    if (argc == 8) { seed = (unsigned int) time(NULL); }
    else if (argc == 9) { seed = (unsigned int) atoi(argv[8]); }
    // checking for correct input arguments
    if (check_input_args(NR_NODES, NR_EDGES, DIST_BINS_PATH, DIST_HIST_PATH, DIST_IDX_PATH, LAMBDA) != 0) { return 1; }
    
    // creating random number generator
    gsl_rng *rng = gsl_rng_alloc(gsl_rng_default);
    gsl_rng_set(rng, seed);

    // reading distance histogram (resulting from distance matrix of brain regions)
    double * distance_bins;
    int * distance_hist;
    int ** distance_indices;
    size_t NR_BINS = 20;
    if (read_distance_histogram(NR_BINS, DIST_BINS_PATH, &distance_bins, DIST_HIST_PATH, &distance_hist, DIST_IDX_PATH, &distance_indices) != 0.0) { return 1; }
    // generate & export random network
    if (LAMBDA == 0.0) // CDR model
    {
        generate_CDR_network(OUTPUT_PATH, NR_NODES, NR_EDGES, NR_BINS, distance_bins, distance_hist, distance_indices, rng);
    }
    else // EDR model
    {
        generate_EDR_network(OUTPUT_PATH, NR_NODES, NR_EDGES, NR_BINS, distance_bins, distance_hist, distance_indices, rng, LAMBDA);
    }
    printf("> SUCCESS: EDR model network with lambda=%lf succesfully exported to: '%s' (RNG seed: %d)\n", LAMBDA, OUTPUT_PATH, seed);

    // cleaning up
    gsl_rng_free(rng);
    free(distance_bins);
    free(distance_hist);
    free(distance_indices);
    
    return 0;
}

int check_input_args(size_t NR_NODES, size_t NR_EDGES, char * DIST_BINS_PATH, char * DIST_HIST_PATH, char * DIST_IDX_PATH, double LAMBDA)
{
    // check for correct values
    if (NR_NODES <= 0) { printf("ERROR: Incorrect value for number of nodes: %ld. Value must be greater than zero.", NR_NODES); return 1; }
    if (NR_EDGES <= 0) { printf("ERROR: Incorrect value for number of edges: %ld. Value must be greater than zero.", NR_EDGES); return 1; }
    FILE * fp = fopen(DIST_BINS_PATH, "r");
    if (fp == NULL) { printf("ERROR: Could not open file with bins of distance histogram '%s'", DIST_BINS_PATH); return 1; }
    fclose(fp);
    fp = fopen(DIST_HIST_PATH, "r");
    if (fp == NULL) { printf("ERROR: Could not open file with bins of distance histogram '%s'", DIST_HIST_PATH); return 1; }
    fclose(fp);
    fp = fopen(DIST_IDX_PATH, "r");
    if (fp == NULL) { printf("ERROR: Could not open file with bins of distance histogram '%s'", DIST_IDX_PATH); return 1; }
    fclose(fp);
    if (LAMBDA < 0) { printf("ERROR: Incorrect value for lambda decay parameter: %lf. Value must be greater or equal than zero.", LAMBDA); return 1; }

    return 0; // all OK
}