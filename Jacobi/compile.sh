#!/bin/bash

echo "Matrix_size: $1 X $1"
echo "" 

g++ input_generate.cpp 
./a.out $1

g++ jacobi.cpp
./a.out

g++ jacobi_omp.cpp -fopenmp
./a.out

nvcc jacobi_cuda.cu -arch=sm_50
./a.out
