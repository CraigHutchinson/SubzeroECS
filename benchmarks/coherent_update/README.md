# Position Update Benchmark

This benchmark compares three different architectural approaches for updating entity positions in a simple physics simulation.

## Implementations

### 1. OOP (Object-Oriented Programming)
**File:** `oop_implementation.hpp`

- Uses a traditional class hierarchy with virtual methods
- `EntityBase` as abstract base class
- `MovingEntity` as concrete implementation
- Entities stored as pointers in a vector
- **Characteristics:**
  - Virtual dispatch overhead for each update call
  - Poor cache locality due to pointer indirection
  - Object-oriented design patterns

### 2. DOD (Data-Oriented Design)
**File:** `dod_implementation.hpp`

- Uses Structure of Arrays (SoA) layout
- Separate contiguous arrays for each component type
- Direct array access without indirection
- **Characteristics:**
  - Excellent cache locality
  - SIMD-friendly memory layout
  - No virtual dispatch
  - Minimal memory overhead

### 3. SubzeroECS (Entity-Component-System)
**File:** `ecs_implementation.hpp`

- Uses SubzeroECS framework
- Components as simple data structures (`Position`, `Velocity`)
- System-based processing (`PhysicsSystem`)
- **Characteristics:**
  - Flexible component composition
  - System-based architecture
  - Cache-friendly iteration
  - Supports complex queries and filtering

## Physics Simulation

All three implementations share the same update logic:

```cpp
// Update position based on velocity
position += velocity * deltaTime

// Apply gravity
velocity.y += 9.8 * deltaTime

// Apply damping (air resistance)
velocity *= 0.99

// Wrap around boundaries (0-1000 range)
if (position.x < 0 || position.x > 1000) wrap(position.x)
if (position.y < 0 || position.y > 1000) wrap(position.y)
```

## Benchmark Scales

The benchmark tests with the following entity counts:
- 10 entities
- 100 entities
- 1,000 entities
- 10,000 entities
- 100,000 entities

## Expected Results

**Theoretical Performance Ranking (fastest to slowest):**

1. **DOD** - Should be the fastest due to:
   - Optimal cache locality with SoA layout
   - No virtual dispatch overhead
   - Minimal abstraction cost
   - SIMD auto-vectorization potential

2. **SubzeroECS** - Should be close to DOD:
   - Cache-friendly component iteration
   - No virtual dispatch in hot paths
   - Small overhead from ECS abstraction

3. **OOP** - Expected to be slowest:
   - Virtual dispatch for every entity
   - Cache misses from pointer chasing
   - Poor memory locality

**Note:** Actual results depend on:
- Compiler optimizations
- CPU architecture and cache sizes
- Entity count (overhead becomes less significant with more entities)

## Running the Benchmark

```bash
# Build in Release mode (critical for accurate results!)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Run the benchmark
./build/benchmarks/position_update/position_update_benchmark

# Or on Windows
.\build\benchmarks\position_update\Release\position_update_benchmark.exe
```

## Interpreting Results

Google Benchmark outputs:
- **Time** - Wall-clock time per iteration
- **CPU** - CPU time per iteration
- **Iterations** - Number of times the benchmark was run
- **Items/s** - Entities processed per second

Example output:
```
---------------------------------------------------------------------------
Benchmark                             Time             CPU   Iterations
---------------------------------------------------------------------------
BM_OOP_UpdatePositions/10          2.15 us         2.14 us       327680   OOP (Virtual Dispatch)
BM_DOD_UpdatePositions/10          0.95 us         0.95 us       747520   DOD (Structure of Arrays)
BM_ECS_UpdatePositions/10          1.12 us         1.12 us       627200   SubzeroECS
```

Lower time = better performance

## Key Takeaways

1. **Cache Locality Matters:** Data layout has a huge impact on performance
2. **Virtual Dispatch Costs:** Can become significant in tight loops
3. **ECS Overhead:** Modern ECS frameworks can match raw DOD performance
4. **Scale Effects:** Performance characteristics change with entity count
