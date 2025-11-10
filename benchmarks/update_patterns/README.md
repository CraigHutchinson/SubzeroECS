# Update Patterns Benchmark

This unified benchmark compares different entity update patterns (Coherent vs Fragmented) across three implementation approaches: Object-Oriented Programming (OOP), Data-Oriented Design (DOD), and Entity Component System (ECS using SubzeroECS).

## Overview

This benchmark consolidates the separate `coherent_update` and `fragmented_update` benchmarks into a single executable for easier comparison and analysis.

## Benchmark Patterns

### Coherent Patterns
- **OOP-Coherent**: All entities are the same type (MovingEntity), using virtual dispatch
- **DOD-Coherent**: Structure of Arrays (SoA) with contiguous memory layout for all entities
- **ECS-Coherent**: All entities have identical component composition (Position + Velocity)

### Fragmented Patterns
- **OOP-Fragmented**: Three entity types with different sizes and processing
  - Small: Position + Velocity (4 floats)
  - Medium: + Health + Rotation + Scale (7 floats)
  - Large: + Color + Team + Flags (11 floats + 2 ints)
- **DOD-Fragmented**: Array of Arrays - separate SoA structures for each entity type
  - SmallEntities, MediumEntities, LargeEntities with matching data layouts
- **ECS-Fragmented**: Component-based entity types with varying compositions
  - Small: Position + Velocity components
  - Medium: + Health + Rotation + Scale components  
  - Large: + Color + Team + Flags components

**All fragmented implementations process identical logic:**
- Small entities: Physics update only
- Medium entities: Physics update + rotation increment + health decrement
- Large entities: Physics update + rotation increment + health decrement

## Entity Sizes Tested

- **10 entities**: Micro-benchmark, warmup
- **1,000 entities**: Small-scale simulation
- **100,000 entities**: Medium-scale, fits in L3 cache
- **10,000,000 entities**: Large-scale, exceeds cache capacity

## Operations Benchmarked

1. **CreateEntities**: Entity creation and component initialization
2. **UpdatePositions**: Physics update loop with entity-specific processing
   - Small entities: Position + velocity integration
   - Medium entities: Position + velocity + rotation increment + health decrement
   - Large entities: Position + velocity + rotation increment + health decrement

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

### Performance Characteristics (10M Entities)

**Coherent Patterns:**
- DOD-Coherent: ~256M items/s (fastest - optimal cache locality with single SoA)
- ECS-Coherent: ~166M items/s (competitive - contiguous component storage)
- OOP-Coherent: ~148M items/s (slower - pointer indirection and virtual calls)

**Fragmented Patterns:**
- DOD-Fragmented: ~256M items/s (maintains performance with separate SoA per type)
- ECS-Fragmented: ~95M items/s (42% slowdown - set intersection overhead across multiple component types)
- OOP-Fragmented: ~73M items/s (51% slowdown - virtual dispatch + heap fragmentation)

**Key Insights:**
- DOD maintains consistent performance in both patterns due to efficient memory layout
- ECS handles fragmentation better than OOP but incurs query overhead
- OOP suffers most from fragmentation due to pointer chasing and cache misses

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
