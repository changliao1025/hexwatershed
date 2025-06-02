#!/bin/bash
# Remove any existing build directory and create a new one
rm -rf build
mkdir build
cd build

# Load required modules
module load cmake/3.11.4
module load gcc/8.1.0

# Run CMake and specify the build directory
cmake ..
make install
