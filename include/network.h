/**
 * @file network.h
 * @author Balazs Pentek (bpentek on github)
 * @brief C source file of EDR random network model generation
 * @version 0.1
 * @date 2024-09-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef NETWORK_H
#define NETWORK_H

#include <stdlib.h>
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
int binary_search(double * bins, size_t nr_bins, double bin_width, double value);

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
int generate_CDR_network(char * output_path, size_t nr_nodes, size_t nr_edges, size_t nr_bins, double * dist_bins, int * dist_hist, int ** dist_indices, gsl_rng * rng);

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
int generate_EDR_network(char * output_path, size_t nr_nodes, size_t nr_edges, size_t nr_bins, double * dist_bins, int * dist_hist, int ** dist_indices, gsl_rng * rng, double lambda);

#endif // NETWORK_H