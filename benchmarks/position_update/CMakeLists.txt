cmake_minimum_required(VERSION 3.14...3.22)

# Create the benchmark executable
add_executable(position_update_benchmark
    main.cpp
    oop_implementation.hpp
    dod_implementation.hpp
    ecs_implementation.hpp
)

# Link against SubzeroECS and Google Benchmark
target_link_libraries(position_update_benchmark
    PRIVATE
        SubzeroECS::SubzeroECS
        benchmark::benchmark
        benchmark::benchmark_main
)

# Set C++ standard
target_compile_features(position_update_benchmark PRIVATE cxx_std_20)

# Enable optimizations for benchmarks
# Note: Benchmarks should always be built in Release mode for accurate results
if(MSVC)
    # Replace default flags to avoid /RTC1 conflict with /O2 in Debug builds
    target_compile_options(position_update_benchmark PRIVATE
        /W4                                  # Warning level 4
        $<$<CONFIG:Debug>:/Od>              # Debug: Disable optimization (use Release preset instead!)
        $<$<CONFIG:Release>:/O2             # Release: Full optimization
            #/Oi                              # Enable intrinsic functions
            #/Ot                              # Favor fast code
            #/GL                             # Whole program optimization
            >
        $<$<CONFIG:RelWithDebInfo>:/O2      # RelWithDebInfo: Full optimization
           # /Oi
           # /Ot
           # /GL
           >
    )
    # Enable link-time optimizations in Release
    target_link_options(position_update_benchmark PRIVATE
        $<$<CONFIG:Release>:/LTCG>          # Link-time code generation
        $<$<CONFIG:RelWithDebInfo>:/LTCG>
    )
    # Disable runtime checks for benchmarks
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        # Warning: benchmarking in Debug is not recommended
        message(WARNING "Building benchmarks in Debug mode. Use Release preset for accurate results!")
    endif()
else()
    # GCC/Clang
    target_compile_options(position_update_benchmark PRIVATE
        $<$<CONFIG:Debug>:-O0>              # Debug: No optimization (use Release preset instead!)
        $<$<CONFIG:Release>:-O3             # Release: Maximum optimization
            -march=native                    # Optimize for this CPU
            -mtune=native                    # Tune for this CPU
            -ffast-math                      # Fast math optimizations
            -flto>                           # Link-time optimization
        -Wall -Wextra                        # Enable warnings
    )
    # Enable link-time optimizations in Release
    target_link_options(position_update_benchmark PRIVATE
        $<$<CONFIG:Release>:-flto>          # Link-time optimization
    )
endif()

# Ensure NDEBUG is defined in Release builds (disables asserts)
target_compile_definitions(position_update_benchmark PRIVATE
    $<$<CONFIG:Release>:NDEBUG>
    $<$<CONFIG:RelWithDebInfo>:NDEBUG>
)
