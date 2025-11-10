# Benchmarks Added to SubzeroECS

## Summary

A new `benchmarks/` folder has been added to the SubzeroECS project with comprehensive performance testing infrastructure.

## What Was Added

### 1. Benchmarks Infrastructure
- **Location:** `benchmarks/`
- **Build System:** CMake with Google Benchmark integration
- **Enable Option:** `-DSUBZEROECS_BUILD_BENCHMARKS=ON`

### 2. Position Update Benchmark
- **Location:** `benchmarks/position_update/`
- **Purpose:** Stress test comparing OOP, DOD, and SubzeroECS approaches
- **Test Scales:** 10, 100, 1K, 10K, 100K entities

### 3. Three Implementation Approaches

#### OOP (Object-Oriented Programming)
- Traditional class hierarchy with virtual methods
- Base class `EntityBase` with virtual `update()`
- Tests overhead of virtual dispatch and pointer indirection

#### DOD (Data-Oriented Design)  
- Structure of Arrays (SoA) layout
- Separate contiguous arrays for each component
- Optimal cache locality and SIMD potential

#### SubzeroECS (Entity-Component-System)
- Uses SubzeroECS framework
- Component-based architecture with `Position` and `Velocity`
- System-based processing with `PhysicsSystem`

### 4. Documentation
- **benchmarks/README.md** - Overview and reference links
- **benchmarks/BUILDING.md** - Build and run instructions
- **benchmarks/position_update/README.md** - Detailed benchmark explanation

## Reference Materials

The README includes links to excellent ECS benchmark resources:
- **abeimler/ecs_benchmark** - Comprehensive C++ ECS framework comparisons
- **EnTT, Flecs, EntityX** - Major ECS frameworks
- **ECS FAQ** - Community resources

## Physics Simulation

All three implementations use identical update logic:
```cpp
position += velocity * deltaTime
velocity.y += gravity * deltaTime  // 9.8 m/s²
velocity *= damping                // 0.99 air resistance
wrap_boundaries(position)          // 0-1000 range
```

## File Structure

```
benchmarks/
├── CMakeLists.txt                 # Main benchmarks build config
├── README.md                      # Overview and references
├── BUILDING.md                    # Build instructions
└── position_update/
    ├── CMakeLists.txt            # Benchmark build config
    ├── README.md                 # Benchmark details
    ├── main.cpp                  # Google Benchmark test cases
    ├── oop_implementation.hpp    # OOP approach
    ├── dod_implementation.hpp    # DOD approach
    └── ecs_implementation.hpp    # SubzeroECS approach
```

## Building

```bash
# Configure with benchmarks enabled
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSUBZEROECS_BUILD_BENCHMARKS=ON

# Build
cmake --build build --config Release

# Run
./build/benchmarks/position_update/position_update_benchmark
```

## Expected Performance

**Theoretical ranking (fastest to slowest):**
1. DOD - Optimal cache locality, no overhead
2. SubzeroECS - ECS abstraction with cache-friendly iteration
3. OOP - Virtual dispatch and pointer chasing overhead

## Next Steps

Future benchmarks could include:
- Entity creation/destruction performance
- Component add/remove operations
- Complex system interactions
- Query performance with filtering
- Memory usage comparisons
