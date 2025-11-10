# Update Patterns Benchmark

This unified benchmark compares different entity update patterns (Coherent vs Fragmented) across three implementation approaches: Object-Oriented Programming (OOP), Data-Oriented Design (DOD), and Entity Component System (ECS using SubzeroECS).

## Overview

This benchmark consolidates the separate `coherent_update` and `fragmented_update` benchmarks into a single executable for easier comparison and analysis.

## Benchmark Patterns

### Coherent Patterns
- **OOP-Coherent**: All entities are the same type, using virtual dispatch
- **DOD-Coherent**: Structure of Arrays (SoA) with contiguous memory layout
- **ECS-Coherent**: All entities have identical component composition (Position + Velocity)

### Fragmented Patterns
- **OOP-Fragmented**: Mixed entity types (Small, Medium, Large) with different sizes, heap fragmentation
- **DOD-Fragmented**: Array of Structures (AoS) with padding to reduce cache efficiency
- **ECS-Fragmented**: Mixed entity compositions - some entities have ExtraData component, others don't

## Entity Sizes Tested

- **10 entities**: Micro-benchmark, warmup
- **1,000 entities**: Small-scale simulation
- **100,000 entities**: Medium-scale, fits in L3 cache
- **10,000,000 entities**: Large-scale, exceeds cache capacity

## Operations Benchmarked

1. **CreateEntities**: Entity creation and component initialization
2. **UpdatePositions**: Physics update loop (position, velocity, gravity, damping, boundary wrapping)

## Shared Physics Logic

All implementations use identical physics calculations from `common.hpp`:
- Position integration: `position += velocity * deltaTime`
- Gravity: `vy += 9.8 * deltaTime`
- Damping: `velocity *= 0.99`
- Boundary wrapping: 0-1000 range

## Building

The benchmark is built as part of the SubzeroECS benchmark suite:

```bash
# Configure with benchmarks enabled
cmake --preset windows-x64-release-benchmark

# Build
cmake --build --preset windows-x64-release-benchmark --target update_patterns_benchmark
```

## Running

```bash
# Run all benchmarks
./update_patterns_benchmark

# Run only ECS benchmarks
./update_patterns_benchmark --benchmark_filter="BM_ECS.*"

# Run only Update operations (skip creation)
./update_patterns_benchmark --benchmark_filter=".*UpdatePositions.*"

# Compare Coherent vs Fragmented for ECS
./update_patterns_benchmark --benchmark_filter="BM_ECS.*UpdatePositions.*"

# Run specific size
./update_patterns_benchmark --benchmark_filter=".*/100000"
```

## Expected Results

### Coherent Patterns
- DOD should be fastest (excellent cache locality)
- ECS should be competitive with DOD
- OOP will be slower due to pointer indirection and virtual calls

### Fragmented Patterns
- Performance degradation compared to coherent patterns
- ECS should handle fragmentation better than OOP (still uses contiguous component storage)
- DOD-Fragmented demonstrates why AoS is less cache-friendly than SoA

### Scaling Behavior
- All patterns show performance decrease from 100K to 10M entities
- This is expected due to cache pressure (100K fits in L3, 10M does not)
- The ratio of performance decrease indicates how well each pattern handles cache misses

## File Structure

```
update_patterns/
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
├── main.cpp                    # Benchmark registration
├── common.hpp                  # Shared utilities and physics logic
├── oop_implementation.hpp      # OOP coherent & fragmented
├── dod_implementation.hpp      # DOD coherent & fragmented
└── ecs_implementation.hpp      # ECS coherent & fragmented
```

## Code Reuse Benefits

1. **Single Physics Implementation**: All benchmarks use the same `Physics::updatePosition()` function
2. **Consistent Setup**: Shared `RandomGenerator` ensures identical test data
3. **Easy Comparison**: Run all patterns in one benchmark session
4. **Reduced Maintenance**: Changes to physics logic or test sizes apply everywhere
5. **Side-by-Side Results**: Direct comparison without switching executables
