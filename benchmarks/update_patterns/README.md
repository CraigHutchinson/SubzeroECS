# Update Patterns Benchmark

This unified benchmark compares different entity update patterns (Coherent vs Fragmented) across three implementation approaches: Object-Oriented Programming (OOP), Data-Oriented Design (DOD), and Entity Component System (ECS using SubzeroECS).

## Overview

This benchmark consolidates the separate `coherent_update` and `fragmented_update` benchmarks into a single executable for easier comparison and analysis.

## Benchmark Patterns

All benchmarks now use a unified implementation approach where the "Coherent" pattern is achieved by using the fragmented implementation with `DistributionPattern::Coherent`, which creates all entities as the Small type.

### Coherent Patterns (All Small Entities)
- **OOP-Coherent**: All entities created as SmallEntity (Position + Velocity only)
- **DOD-Coherent**: All entities in SmallEntities SoA structure  
- **ECS-Coherent**: All entities with Position + Velocity components only

### Fragmented Patterns (Mixed Entity Types)
- **OOP-Fragmented**: Three entity types distributed evenly (1/3 each)
  - Small: Position + Velocity (4 floats)
  - Medium: + Health + Rotation + Scale (7 floats)
  - Large: + Color + Team + Flags (11 floats + 2 ints)
- **DOD-Fragmented**: Separate SoA structures for each type (1/3 each)
  - SmallEntities, MediumEntities, LargeEntities with matching data layouts
- **ECS-Fragmented**: Component-based entity types with varying compositions (1/3 each)
  - Small: Position + Velocity components
  - Medium: + Health + Rotation + Scale components  
  - Large: + Color + Team + Flags components

**All implementations process identical logic using shared functions from `common.hpp`:**
- Small entities: `Physics::updatePosition()` only
- Medium entities: `Physics::updatePosition()` + `Physics::updateRotationHealth()`
- Large entities: `Physics::updatePosition()` + `Physics::updateRotationHealth()` + `Physics::pulseScale()`

## Distribution Patterns

The benchmark uses the `getEntityType()` function from `common.hpp` to control entity distribution:

- **Coherent**: `getEntityType(i, DistributionPattern::Coherent)` returns `EntityType::Small` for all entities
- **Fragmented**: `getEntityType(i, DistributionPattern::Fragmented)` rotates through Small/Medium/Large (33% each)

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
- **`Physics::updatePosition()`**: Position integration with gravity, damping, and boundary wrapping
  - `position += velocity * deltaTime`
  - `vy += 9.8 * deltaTime` (gravity)
  - `velocity *= 0.99` (damping)
  - Boundary wrapping: 0-1000 range
- **`Physics::updateRotationHealth()`**: Rotation increment and health decrement
  - `rotation += 0.5 * deltaTime`
  - `health -= 0.1 * deltaTime`
- **`Physics::pulseScale()`**: Scale and color pulsing for large entities
  - Scale oscillates based on time
  - RGB color channels pulse in sync with scale

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

**Coherent Patterns (All Small Entities):**
- ECS-Coherent: ~163M items/s
- OOP-Coherent: ~160M items/s  
- DOD-Coherent: ~90M items/s

**Fragmented Patterns (Mixed Entity Types):**
- DOD-Fragmented: ~198M items/s (fastest - separated update loops optimize cache usage)
- ECS-Fragmented: ~110M items/s (good - component-based architecture handles mixed types well)
- OOP-Fragmented: ~84M items/s (slowest - virtual dispatch + heap fragmentation)

**Key Insights:**
- DOD Fragmented performs best overall due to separated update loops (multiple passes over smaller data sets)
- Coherent pattern shows different performance across implementations - not universally faster
- DOD Coherent is slower than expected because all entities are Small but Medium/Large structures still exist (though empty)
- ECS and OOP show similar relative performance between coherent and fragmented patterns
- Processing mixed entity types doesn't necessarily hurt performance if cache utilization is optimized

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
├── common.hpp                  # Shared utilities, EntityType enum, DistributionPattern, and physics logic
├── oop_implementation.hpp      # OOP implementation (SmallEntity, MediumEntity, LargeEntity)
├── dod_implementation.hpp      # DOD implementation (SmallEntities, MediumEntities, LargeEntities SoA)
└── ecs_implementation.hpp      # ECS implementation (component-based with systems)
```

## Code Reuse Benefits

1. **Single Implementation Per Pattern**: Coherent benchmarks reuse fragmented implementations with `DistributionPattern::Coherent`
2. **Shared Physics Implementation**: All benchmarks use the same `Physics` functions from `common.hpp`
3. **Type-Safe Entity Distribution**: `EntityType` enum ensures all implementations create identical entity mixes
4. **Consistent Setup**: Shared `RandomGenerator` ensures identical test data
5. **Easy Comparison**: Run all patterns in one benchmark session
6. **Reduced Maintenance**: Changes to physics logic or test sizes apply everywhere
7. **Side-by-Side Results**: Direct comparison without switching executables
