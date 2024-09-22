/**
 * @file distance.h
 * @author Balazs Pentek (bpentek on github)
 * @brief C header file of distance matrix histogram generation for the EDR model.
 * @version 0.1
 * @date 2024-09-22
 * 
 * 
 */

#ifndef DISTANCE_H
#define DISTANCE_H

#include <stdlib.h>

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
int export_distance_histogram(char * output_root, double ** distances, size_t nr_nodes, int is_symm, size_t nr_bins);

/**
 * @brief Function that reads distance matrix
 * 
 * @param matrix_path Path to distance matrix (stored in edgelist format: "row col distance\n")
 * @param distances Distance matrix
 * @param nr_nodes Total number of nodes/areas (matrix dim)
 * @return int Success/error code (0 or 1)
 */
int read_distance_matrix(char * matrix_path, double *** distances, size_t nr_nodes);

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
int read_distance_histogram(size_t nr_bins, char * bins_path, double ** bins, char * hist_path, int ** hist, char * indices_path, int *** indices);

#endif // DISTANCE_H