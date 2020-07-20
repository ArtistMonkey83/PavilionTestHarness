#!/bin/bash
#
# This script will quickly compile and run the hello_world.c and
# hello_world.f90 tests. It assumes that mpicc, mpif90, and mpicc
# are already in PATH.
#
# This script will attempt to run the tests in the same way that
# ../openmpi_build_test.bash runs the tests.

# Run hostname on two processes
echo ""
echo "Running hostname on two processes..."
mpirun --map-by ppr:1:node hostname

# Make sure there isn't a compiled binary already
rm -f hello_world

# Compile and run the C version
echo ""
echo "Compiling C program..."
mpicc -o hello_world_c hello_world.c
if [[ $? == 0 ]]; then
    echo "Running C program, 1 MPI rank per socket..."
    mpirun --map-by ppr:1:socket ./hello_world_c
fi
rm -f hello_world_c

# Compile and run the Fortran90 version
echo ""
echo "Compiling Fortran90 program..."
mpif90 -o hello_world_f hello_world.f90
if [[ $? == 0 ]]; then
    echo "Running Fortran90 program, 1 MPI rank per socket..."
    mpirun --map-by ppr:1:socket ./hello_world_f
fi
rm -f hello_world_f
