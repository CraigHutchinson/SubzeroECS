cmake_minimum_required(VERSION 3.14...3.22)

project(SubzeroECSBenchmarks CXX)

# Find Google Benchmark (using cmake/FindBenchmark.cmake)
find_package(Benchmark REQUIRED)

# Add benchmark subdirectories
add_subdirectory(coherent_update)
add_subdirectory(fragmented_update)
