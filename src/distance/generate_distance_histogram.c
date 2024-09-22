/**
 * @file generate_distance_histogram.c
 * @author bpentek
 * @brief C program that generates histogram of distance matrix used by the EDR random network model.
 * @version 0.1
 * @date 2024-09-22
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "distance.h"

int main(int argc, char * argv[])
{
    // input args
    if (argc != 6)
    {
        printf("USAGE: %s </PATH/TO/DISTANCE/MATRIX> <NUMBER OF NODES/AREAS (int)> <NUMBER OF BINS (int)> <SYMMETRIC MATRIX OR NOT (1 or 0)> </PATH/TO/OUTPUT/ROOT/DIR/>", argv[0]);
        return 1;
    }
    char * DIST_MATRIX_PATH = argv[1];
    size_t NR_NODES = (size_t) atoi(argv[2]);
    size_t NR_BINS = (size_t) atoi(argv[3]);
    int SYMMETRIC = atoi(argv[4]);
    char * OUTPUT_ROOT = argv[5];
    
    double ** distances;
    // reading distance matrix
    if (read_distance_matrix(DIST_MATRIX_PATH, &distances, NR_NODES) != 0) { return 1; }
    // exporting histogram of distance matrix
    if (export_distance_histogram(OUTPUT_ROOT, distances, NR_NODES, SYMMETRIC, NR_BINS) != 0) { return 1; }

    printf("> SUCCESS: Histogram of distance matrix exported to: '%s' (distance_bins.txt, distance_hist.txt, distance_indices.txt files)", OUTPUT_ROOT);

    // cleaning up
    for (size_t i = 0; i < NR_NODES; i++) free(distances[i]);
    free(distances);

    return 0;
}