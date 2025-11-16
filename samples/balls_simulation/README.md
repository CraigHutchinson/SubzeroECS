# 2D Balls Simulation - SubzeroECS Demo

An advanced physics simulation demonstrating different architectural patterns for game development, with real-time performance comparison.

## Project Structure

The simulation is organized into separate, focused files with shared common code:

- **`Components.hpp`**: Shared component definitions and physics configuration
- **`PhysicsCommon.hpp`**: Common physics functions used by all implementations
  - Gravity application
  - Position/velocity integration
  - Wall collision handling
  - Ball collision detection and resolution
  - Damping functions
- **`ECS_Systems.hpp`**: SubzeroECS implementation with four systems:
  - `GravitySystem`: Applies gravity to entities
  - `MovementSystem`: Integrates velocity into position
  - `BoundaryCollisionSystem`: Handles wall collisions
  - `BallCollisionSystem`: Handles ball-to-ball collisions
- **`SoA_Implementation.hpp`**: Structure of Arrays (DOD) implementation
- **`AoS_Implementation.hpp`**: Array of Structures implementation
- **`OOP_Implementation.hpp`**: Object-Oriented Programming implementation
- **`main.cpp`**: Application entry point with SFML rendering and UI
- **`AlternativePatterns.hpp`**: Legacy file (retained for backward compatibility)

All implementations share the same physics functions from `PhysicsCommon.hpp`, ensuring identical behavior for fair performance comparison.

## Features

- **Multiple Implementation Patterns**: Switch between different implementations at runtime
  - **ECS (Entity Component System)**: Using SubzeroECS
  - **DOD (Data-Oriented Design)**: Structure of Arrays (SoA)
  - **Array of Structures (AoS)**: Traditional C-style approach
  - **OOP (Object-Oriented Programming)**: Class-based with virtual methods
  
- **Realistic Physics**:
  - Gravity simulation
  - Elastic collisions between balls
  - Boundary collisions with walls
  - Energy conservation with configurable restitution
  - Friction and damping

- **Real-Time Performance Metrics**:
  - FPS (Frames Per Second)
  - Update time in milliseconds
  - Processing throughput (millions of items per second)
  - Entity count

- **Interactive Controls**:
  - Dynamically add/remove entities
  - Switch implementation patterns on-the-fly
  - Real-time performance comparison

## Controls

| Key | Action |
|-----|--------|
| **1** | Switch to ECS (SubzeroECS) |
| **2** | Switch to DOD (Structure of Arrays) |
| **3** | Switch to Array of Structures |
| **4** | Switch to OOP (Object-Oriented) |
| **SPACE** | Add 10 random balls |
| **C** | Clear all balls |
| **R** | Reset with 100 balls |
| **ESC** | Exit application |

## Building

### Prerequisites

- CMake 3.14 or higher
- C++20 compatible compiler
- SFML 3.0.2 (automatically downloaded via CPM)

### Build Instructions

```bash
# From SubzeroECS root directory
cmake --preset windows-x64-release
cmake --build --preset windows-x64-release --target BallsSimulation
```

### Running

```bash
# From build output directory
./out/Windows-build/windows-x64-release/samples/balls_simulation/Release/BallsSimulation.exe
```

## Performance Characteristics

### Expected Performance Patterns

**ECS (SubzeroECS)**:
- ✅ Cache-friendly data layout
- ✅ Efficient iteration over entities with specific components
- ✅ Good scalability with entity count
- ⚠️ Slight overhead from view intersection logic

**DOD (Structure of Arrays)**:
- ✅ Best cache utilization
- ✅ Excellent SIMD potential
- ✅ Fastest for simple, uniform operations
- ⚠️ Less flexible for heterogeneous entities

**Array of Structures**:
- ⚠️ Moderate cache efficiency
- ⚠️ Each entity's data is contiguous, but not all positions/velocities
- ✅ Simple to understand and implement

**OOP (Object-Oriented)**:
- ❌ Poor cache locality (pointer chasing)
- ❌ Virtual function call overhead
- ❌ Worst scalability
- ✅ Most familiar pattern for many developers

### Typical Results (at ~1000 entities)

| Pattern | Update Time | Items/s | Cache Efficiency |
|---------|-------------|---------|------------------|
| **DOD (SoA)** | ~0.5 ms | ~2000 M items/s | ⭐⭐⭐⭐⭐ |
| **ECS** | ~0.8 ms | ~1250 M items/s | ⭐⭐⭐⭐ |
| **AoS** | ~1.2 ms | ~830 M items/s | ⭐⭐⭐ |
| **OOP** | ~2.5 ms | ~400 M items/s | ⭐⭐ |

*Note: Actual performance depends on hardware, compiler optimizations, and entity count.*

## Implementation Details

### Physics Simulation

The simulation implements:
- **Gravity**: Constant downward acceleration (~980 px/s²)
- **Elastic Collisions**: Ball-to-ball collisions with momentum conservation
- **Boundary Response**: Walls with configurable restitution and friction
- **Damping**: Energy dissipation over time

### Collision Detection

Currently uses brute-force O(n²) collision detection for simplicity and fair comparison across all patterns. In production, you would use:
- Spatial partitioning (quadtree, grid)
- Broad-phase/narrow-phase separation
- SIMD optimizations for distance calculations

### Architecture Comparison

**ECS Pattern** (`ECS_Systems.hpp`):
```cpp
class GravitySystem : public SubzeroECS::System<GravitySystem, Velocity, Mass> {
    void processEntity(Iterator it) {
        Velocity& vel = it.get<Velocity>();
        vel.dy += gravity * deltaTime;
    }
};
```

**DOD Pattern** (`AlternativePatterns.hpp`):
```cpp
void applyGravity(float deltaTime) {
    for (size_t i = 0; i < balls.count; ++i) {
        balls.velocities_dy[i] += config.gravity * deltaTime;
    }
}
```

**OOP Pattern** (`AlternativePatterns.hpp`):
```cpp
class Ball {
    void applyGravity(float deltaTime) {
        dy += config->gravity * deltaTime;
    }
};
```

## Educational Value

This sample demonstrates:
1. **Data-oriented design principles** and their performance impact
2. **Cache-friendly memory layouts** (SoA vs AoS)
3. **ECS architecture benefits** for game development
4. **Trade-offs** between flexibility, readability, and performance
5. **Real-time performance measurement** techniques

## Extending the Simulation

Ideas for enhancement:
- Add spatial partitioning for O(n log n) collision detection
- Implement different collision response models
- Add user interaction (click to add balls, drag to apply forces)
- Visualize performance metrics graphically
- Add profiling markers to identify bottlenecks
- Support different ball sizes/densities
- Add constraints (springs, joints)

## License

Part of SubzeroECS project. See main LICENSE file for details.
