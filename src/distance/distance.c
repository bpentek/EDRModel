/**
 * @file distance.c
 * @author bpentek
 * @brief C source file of distance matrix histogram generation for the EDR model.
 * @version 0.1
 * @date 2024-09-22
 * 
 * 
 */

#include "distance.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>

/**
 * @brief Function that exports histogram of distance matrix (representing distances between node/area pairs)
 * 
 * @param output_root Path to output root directory (where 3 separate files will be exported)
 * @param distances Distance matrix (from measurements between node/area pairs)
 * @param nr_nodes Total number of nodes/areas
 * @param is_symm Distance matrix is symmetric (1) or not (0)
 * @param nr_bins Number of bins of the generated histogram
 * @return int Success/error code (0/1)
 */
int export_distance_histogram(char * output_root, double ** distances, size_t nr_nodes, int is_symm, size_t nr_bins)
{
    double min_bins = 0.0, max_bins = 0.0;
    if (is_symm == 1)
    {   
        for (size_t i = 0; i < nr_nodes; i++)
        {
            for (size_t j = i + 1; j < nr_nodes; j++)
            {
                if (distances[i][j] < min_bins) { min_bins = distances[i][j]; }
                if (distances[i][j] > max_bins) { max_bins = distances[i][j]; }
            }
        }
    }
    else if (is_symm == 0)
    {
        for (size_t i = 0; i < nr_nodes; i++)
        {
            for (size_t j = 0; j < nr_nodes; j++)
            {
                if (distances[i][j] < min_bins) { min_bins = distances[i][j]; }
                if (distances[i][j] > max_bins) { max_bins = distances[i][j]; }
            }
        }
    }
    else { printf("ERROR: Incorrect value for `is_symm`:'%d'. Only two values allowed: 1 and 0 (for symmetric/not symmetric distance matrix)", is_symm); return 1;}


    // creating bins for histogram
    double * bins = malloc((nr_bins + 1)*sizeof(double));
    if (bins == NULL) { printf("\nERROR: Failed to allocate memory for distance histogram bins.\n"); return 1; }
    double bin_width = (double) (max_bins - min_bins)/nr_bins;
    for (size_t i = 0; i < nr_bins + 1; i++)
    {
        bins[i] = min_bins + i*bin_width;
    }
    
    // computing histogram of distance matrix
    int * hist = malloc((nr_bins + 1)*sizeof(int));
    if (hist == NULL) { printf("\nERROR: Failed to allocate memory for distance histogram values.\n"); return 1; }
    for (size_t k = 0; k < nr_bins + 1; k++){ hist[k] = 0.0; }
    size_t idx_bins = 0;
    if (is_symm == 1)
    {   
        for (size_t i = 0; i < nr_nodes; i++)
        {
            for (size_t j = i + 1; j < nr_nodes; j++)
            {
                if (distances[i][j] != 0.0)
                {
                    idx_bins = (size_t)floor((distances[i][j] - min_bins)/bin_width);
                    if (distances[i][j] == bins[nr_bins]) idx_bins--;
                    hist[idx_bins]++;
                }
            }
        }
    }
    else if (is_symm == 0)
    {
        for (size_t i = 0; i < nr_nodes; i++)
        {
            for (size_t j = 0; j < nr_nodes; j++)
            {
                if (distances[i][j] != 0.0)
                {
                    idx_bins = (size_t)floor((distances[i][j] - min_bins)/bin_width);
                    if (distances[i][j] == bins[nr_bins]) idx_bins--;
                    hist[idx_bins]++;
                }
            }
        }
    }
    

    //
    int ** indices = malloc((nr_bins + 1)*sizeof(int *));
    if (indices == NULL) { printf("\nERROR: Failed to allocate memory for distance bin indices.\n"); return 1; }
    for (size_t k = 0; k < (nr_bins + 1); k++) { indices[k] = malloc(2*hist[k]*sizeof(int)); }
    size_t idx_edges;

    for (size_t k = 0; k < (nr_bins + 1); k++)
    {
        idx_edges = 0;
        if (is_symm == 1)
        {   
            for (size_t i = 0; i < nr_nodes; i++)
            {
                for (size_t j = i + 1; j < nr_nodes; j++)
                {
                    if (distances[i][j] != 0.0)
                    {
                        idx_bins = (size_t)floor((distances[i][j] - min_bins)/bin_width);
                        if (distances[i][j] == bins[nr_bins]) idx_bins--;
                        if (idx_bins == k)
                        {
                            indices[idx_bins][idx_edges++] = i;
                            indices[idx_bins][idx_edges++] = j;
                        }
                    }
                }
            }
        }
        else if (is_symm == 0)
        {
            for (size_t i = 0; i < nr_nodes; i++)
            {
                for (size_t j = 0; j < nr_nodes; j++)
                {
                    if (distances[i][j] != 0.0)
                    {
                        idx_bins = (size_t)floor((distances[i][j] - min_bins)/bin_width);
                        if (distances[i][j] == bins[nr_bins]) idx_bins--;
                        if (idx_bins == k)
                        {
                            indices[idx_bins][idx_edges++] = i;
                            indices[idx_bins][idx_edges++] = j;
                        }
                    }
                }
            }
        }
    }
    
    // EXPORTING DISTANCE HISTOGRAM
    // file paths
    char * bins_path = malloc(strlen(output_root) + strlen("distance_bins.txt") + 1);
    strcpy(bins_path, output_root);
    strcat(bins_path, "distance_bins.txt");
    char * hist_path = malloc(strlen(output_root) + strlen("distance_hist.txt") + 1);
    strcpy(hist_path, output_root);
    strcat(hist_path, "distance_hist.txt");
    char * indices_path = malloc(strlen(output_root) + strlen("distance_indices.txt") + 1);
    strcpy(indices_path, output_root);
    strcat(indices_path, "distance_indices.txt");
    // files
    FILE * bins_fp = fopen(bins_path, "w");
    if (bins_fp == NULL) { printf("\nERROR: Could not open file with bins of distance histogram '%s'\n", bins_path); return 1; }
    FILE * hist_fp = fopen(hist_path, "w");
    if (hist_fp == NULL) { printf("\nERROR: Could not open file with bins of distance histogram '%s'\n", hist_path); return 1; }
    FILE * indices_fp = fopen(indices_path, "w");
    if (indices_fp == NULL) { printf("\nERROR: Could not open file with bins of distance histogram '%s'\n", indices_path); return 1; }

    // exporting
    for (size_t i = 0; i < nr_bins + 1; i++)
    {
        fprintf(bins_fp, "%lf\n", bins[i]);
        fprintf(hist_fp, "%d\n", hist[i]);
        for (size_t j = 0; j < (size_t) 2*hist[i]; j++)
        {
            fprintf(indices_fp, "%d ", indices[i][j]);
        }
        fprintf(indices_fp, "\n");
    }

    // CLEANING UP
    fclose(bins_fp); fclose(hist_fp); fclose(indices_fp);
    free(bins_path); free(hist_path); free(indices_path);
    free(bins); free(hist);
    for (size_t i = 0; i < nr_bins + 1; i++) { free(indices[i]); }
    free(indices);

    return 0; // all OK
}

/**
 * @brief Function that reads distance matrix
 * 
 * @param matrix_path Path to distance matrix (stored in edgelist format: "row col distance\n")
 * @param distances Distance matrix
 * @param nr_nodes Total number of nodes/areas (matrix dim)
 * @return int Success/error code (0 or 1)
 */
int read_distance_matrix(char * matrix_path, double *** distances, size_t nr_nodes)
{
    (*distances) = malloc(nr_nodes*sizeof(double *));
    if ((*distances) == NULL) { printf("\nERROR: Failed to allocate memory for distance matrix.\n"); return 1; }
    for (size_t i = 0; i < nr_nodes; i++)
    { 
        (*distances)[i] = malloc(nr_nodes*sizeof(double)); 
        if ((*distances)[i] == NULL) { printf("\nERROR: Failed to allocate memory for distance matrix.\n"); return 1; }
    }
    FILE * fp = fopen(matrix_path, "r");
    if (fp == NULL) { printf("\nERROR: Could not open file with distance matrix '%s'\n", matrix_path); return 1; }
    size_t source, target;
    double distance;
    while (fscanf(fp, "%ld %ld %lf", &source, &target, &distance) == 3)
    {
        if (source < 0 || source >= nr_nodes) { printf("\nERROR: Incorrect node index encountered: %ld. Value must be in the [0, %ld) interval.\n", source, nr_nodes); return 1;}
        if (target < 0 || target >= nr_nodes) { printf("\nERROR: Incorrect node index encountered: %ld. Value must be in the [0, %ld) interval.\n", target, nr_nodes); return 1;}
        (*distances)[source][target] = distance;
    }
    fclose(fp);
    return 0;
}

/**
 * @brief Function that reads histogram of distance matrix from files (bins, histogram values and node/area pair indices from separate files)
 * 
 * @param nr_bins Number of bins of the generated histogram
 * @param bins_path Path to histogram bins
 * @param bins Array used to store bins
 * @param hist_path Path to histogram values
 * @param hist Array used to store histogram values
 * @param indices_path Path to node/area pair indices
 * @param indices (2D) Array used to store node/area pair indices
 * @return int 
 */
int read_distance_histogram(size_t nr_bins, char * bins_path, double ** bins, char * hist_path, int ** hist, char * indices_path, int *** indices)
{
    // files
    FILE * bins_fp = fopen(bins_path, "r");
    if (bins_fp == NULL) { printf("\nERROR: Could not open file with bins of distance histogram '%s'\n", bins_path); return 1; }
    FILE * hist_fp = fopen(hist_path, "r");
    if (hist_fp == NULL) { printf("\nERROR: Could not open file with bins of distance histogram '%s'\n", hist_path); return 1; }
    double bin;
    int count;

    (*bins) = malloc((nr_bins + 1)*sizeof(double));
    if ((*bins) == NULL) { printf("\nERROR: Failed to allocate memory for distance bins."); return 1; }
    (*hist) = malloc((nr_bins + 1)*sizeof(int));
    if ((*hist) == NULL) { printf("\nERROR: Failed to allocate memory for distance histogram."); return 1; }
    bins_fp = fopen(bins_path, "r");
    if (bins_fp == NULL) { printf("\nERROR: Could not open file with bins of distance histogram '%s'\n", bins_path); return 1; }
    hist_fp = fopen(hist_path, "r");
    if (hist_fp == NULL) { printf("\nERROR: Could not open file with bins of distance histogram '%s'\n", hist_path); return 1; }    
    for (size_t i = 0; i < (nr_bins + 1); i++)
    {
        fscanf(bins_fp, "%lf", &bin);
        fscanf(hist_fp, "%d", &count);
        (*bins)[i] = bin;
        (*hist)[i] = count;
    }
    fclose(bins_fp); fclose(hist_fp);

    
    (*indices) = malloc((nr_bins + 1)*sizeof(int *));
    if ((*indices) == NULL) { printf("\nERROR: Failed to allocate memory for distance index pairs."); return 1; }
    for (size_t i = 0; i < (nr_bins + 1); i++)
    {
        (*indices)[i] = malloc((2*(*hist)[i])*sizeof(int));
        if ((*indices)[i] == NULL) { printf("\nERROR: Failed to allocate memory for distance index pairs."); return 1; }
    }
    FILE * indices_fp = fopen(indices_path, "r");
    if (indices_fp == NULL) { printf("\nERROR: Could not open file with bins of distance histogram '%s'\n", indices_path); return 1; }
    int idx;
    for (size_t i = 0; i < (nr_bins + 1); i++)
    {
        for (size_t j = 0; j < (2*(*hist)[i]); j++)
        {
            fscanf(indices_fp, "%d", &idx);
            (*indices)[i][j] = idx;
        }
    }
    fclose(indices_fp);

    return 0;
}