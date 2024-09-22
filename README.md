# EDRModel

Script for preparing data & executing EDR random network model generation developed by Ercsey-Ravasz et al., 2013.

## Table of Contents
- [EDRModel](#edrmodel)
  - [Table of Contents](#table-of-contents)
  - [Installation](#installation)
  - [Usage](#usage)

## Installation

To install and build the EDRModel project, follow these steps:

1. **Prerequisites**:
   - Make sure you have the following software installed on your system:
     - CMake (version 3.10 or higher)
     - A C compiler (e.g., GCC, Clang)
     - The GNU Scientific Library (GSL) development package

2. **Clone the repository**:
   `git clone https://github.com/bpentek/EDRModel.git`
   `cd EDRModel`

3. **Create a build directory**:
   `mkdir build`
    `cd build`

4. **Configure the project with CMake**:
    `cmake ..`

5. **Build the project:**
   `cmake --build`

   This will compile the source code and create the two executables: `generate_distance_histogram` and `generate_network` in the `build/` directory. 

## Usage

    To generate an EDR model network, first a histogram needs to be created from the $D_{ij}$ distance matrix values. This can be achieved by using:

    `./build/generate_distance_histogram </PATH/TO/DISTANCE/MATRIX> <NUMBER OF NODES/AREAS (int)> <NUMBER OF BINS (int)> <SYMMETRIC MATRIX OR NOT (1 or 0)> </PATH/TO/OUTPUT/ROOT/DIR/>`

    Once the histogram is created, the EDR model network with a given $\lambda$ parameter can be created like:

    `./build/generate_network <NUMBER OF NODES (int)> <NUMBER OF EDGES (int)> </PATH/TO/DISTANCE/HISTOGRAM/BINS> </PATH/TO/DISTANCE/HISTOGRAM/VALUES> </PATH/TO/DISTANCE/HISTOGRAM/INDICES> <LAMBDA DECAY PARAMETER (double)> </PATH/TO/OUTPUT/EDGELIST> <RANDOM NUMBER GENERATOR SEED [optional]>` 