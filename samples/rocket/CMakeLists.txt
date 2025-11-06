cmake_minimum_required(VERSION 3.14)

project(RocketSample LANGUAGES CXX)

# Create executable
add_executable(RocketSample
  main.cpp
)

# Link against SubzeroECS
target_link_libraries(RocketSample 
  PRIVATE 
    SubzeroECS
)

# Set C++20 standard
set_target_properties(RocketSample 
  PROPERTIES 
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Copy executable to samples output directory
set_target_properties(RocketSample 
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/samples"
)
