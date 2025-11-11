# SubzeroECS

A high-performance, cache-friendly Entity Component System (ECS) framework for modern C++20 applications.

## Features

- **Modern C++20**: Leverages latest C++ features for type safety and performance
- **Cache-Friendly Design**: Structure of Arrays (SoA) component storage for optimal cache utilization
- **Flexible Queries**: Powerful view and query system for efficient entity iteration
- **Type-Safe Components**: Compile-time type checking for component access
- **Minimal Fragmentation Impact**: Handles mixed entity compositions efficiently
- **Zero-Cost Abstractions**: CRTP-based systems with minimal runtime overhead
- **Header-Only Core**: Easy integration into existing projects

## Quick Start

```cpp
#include "SubzeroECS/World.hpp"
#include "SubzeroECS/System.hpp"

// Define components
struct Position { float x, y; };
struct Velocity { float dx, dy; };

// Create a system
class PhysicsSystem : public SubzeroECS::System<PhysicsSystem, Position, Velocity> {
public:
    float deltaTime = 0.0f;
    
    PhysicsSystem(SubzeroECS::World& world)
        : SubzeroECS::System<PhysicsSystem, Position, Velocity>(world) {}

    void processEntity(Iterator iEntity) {
        Position& pos = iEntity.get<Position>();
        Velocity& vel = iEntity.get<Velocity>();
        
        pos.x += vel.dx * deltaTime;
        pos.y += vel.dy * deltaTime;
    }
};

// Use the ECS
int main() {
    SubzeroECS::World world;
    SubzeroECS::Collection<Position, Velocity> collections(world);
    PhysicsSystem physics(world);
    
    // Create entities
    world.create(Position{0.0f, 0.0f}, Velocity{1.0f, 1.0f});
    world.create(Position{10.0f, 5.0f}, Velocity{-0.5f, 2.0f});
    
    // Update
    physics.deltaTime = 1.0f / 60.0f;
    physics.update();
    
    return 0;
}
```

## Performance

SubzeroECS demonstrates excellent performance characteristics, particularly in handling fragmented entity compositions. Benchmark results comparing SubzeroECS against traditional OOP and DOD approaches:

**Test System:**
- CPU: Intel Core i7-11800H @ 2.30GHz (8 cores, 16 logical processors)
- L1 Data Cache: 48 KiB × 8
- L1 Instruction Cache: 32 KiB × 8
- L2 Cache: 1280 KiB × 8
- L3 Cache: 24576 KiB (24 MB)
- Compiler: MSVC with `/O2` optimization
- OS: Windows
- Methodology: Median of 10 repetitions (`--benchmark_repetitions=10`)

### Update Performance at 100K Entities

| Implementation | Coherent (M items/s) | Fragmented (M items/s) | Impact |
|----------------|---------------------|------------------------|--------|
| **SubzeroECS** | 193.39 ± 6.29       | 194.62 ± 4.35          | **+1%** (noise) ✓ |
| **OOP**        | 271.68 ± 4.36       | 259.33 ± 6.15          | -5% |
| **DOD (SoA)**  | 374.35 ± 115.62     | 486.17 ± 14.67         | +30% (high variance) |

### Update Performance at 10M Entities

| Implementation | Coherent (M items/s) | Fragmented (M items/s) | Impact |
|----------------|---------------------|------------------------|--------|
| **SubzeroECS** | 151.21 ± 2.25       | 153.69 ± 4.02          | **+2%** (noise) ✓ |
| **OOP**        | 126.78 ± 2.63       | 85.35 ± 1.27           | **-33%** |
| **DOD (SoA)**  | 210.42 ± 7.98       | 196.88 ± 26.62         | -6% |

**Key Findings:**
- SubzeroECS shows **zero to minimal performance degradation** with mixed entity compositions (fragmentation)
- Traditional OOP suffers **33% slowdown** at 10M scale with heterogeneous entity types
- Pure DOD Structure-of-Arrays is faster in absolute terms (~1.9x at 100K, ~1.4x at 10M) but SubzeroECS provides better developer ergonomics with competitive performance
- The improved N-way intersection algorithm (galloping search with adaptive linear/binary switching) maintains consistent performance across different entity compositions
- Standard deviations are reported (±) showing measurement reliability; SubzeroECS demonstrates low variance (CV < 3.3%) across all scenarios

See [benchmarks/update_patterns](benchmarks/update_patterns) for detailed benchmark code and methodology.

## Building SubzeroECS

### Using CMake Presets (Recommended)

```bash
# Configure for release build
cmake --preset windows-x64-release

# Build the library
cmake --build --preset windows-x64-release

# Run tests
ctest --preset windows-x64-release
```

### Available Presets

- `windows-x64-debug` - Debug build with assertions
- `windows-x64-release` - Optimized release build
- `windows-x64-release-benchmark` - Release build with benchmarks enabled

### Building from Source

```bash
# Clone the repository
git clone https://github.com/CraigHutchinson/SubzeroECS.git
cd SubzeroECS

# Configure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run tests
cd build/test
ctest --output-on-failure
```

## Project Structure

```
SubzeroECS/
├── source/SubzeroECS/          # Core ECS implementation
│   ├── World.hpp               # Entity and world management
│   ├── Collection.hpp          # Component storage (SoA)
│   ├── System.hpp              # System base class (CRTP)
│   ├── View.hpp                # Multi-component queries
│   ├── Entity.hpp              # Entity handle
│   └── Utility/                # Helper utilities
├── samples/                    # Example applications
│   └── rocket/                 # Physics simulation example
├── benchmarks/                 # Performance benchmarks
│   └── update_patterns/        # Unified coherent vs fragmented comparison
└── test/                       # Unit tests
```

## Usage

### Running Tests

Use the following commands from the project's root directory to run the test suite.

```bash
# Using presets
ctest --preset windows-x64-release

# Or manually
cmake -S test -B build/test
cmake --build build/test
./build/test/SubzeroECSTests
```

### Running Benchmarks

```bash
# Build benchmarks
cmake --preset windows-x64-release-benchmark
cmake --build --preset windows-x64-release-benchmark

# Run unified update patterns benchmark
./out/Windows-build/windows-x64-release-benchmark/benchmarks/update_patterns/Release/update_patterns_benchmark.exe

# Filter specific tests
./update_patterns_benchmark.exe --benchmark_filter="BM_ECS.*"
./update_patterns_benchmark.exe --benchmark_filter=".*UpdatePositions.*/100000"
```

### Running Samples

```bash
# Build and run the rocket physics sample
cmake --build --preset windows-x64-release --target RocketSample
./out/Windows-build/windows-x64-release/samples/rocket/Release/RocketSample.exe
```

## Development

### Code Formatting

Use the following commands from the project's root directory to check and fix C++ and CMake source style.
This requires _clang-format_, _cmake-format_ and _pyyaml_ to be installed on the current system.

```bash
cmake -S test -B build/test

# view changes
cmake --build build/test --target format

# apply changes
cmake --build build/test --target fix-format
```

See [Format.cmake](https://github.com/TheLartians/Format.cmake) for details.
These dependencies can be easily installed using pip.

```bash
pip install clang-format==14.0.6 cmake_format==0.6.11 pyyaml
```

### Static Analysis

Static Analyzers can be enabled by setting `-DUSE_STATIC_ANALYZER=<clang-tidy | iwyu | cppcheck>`, or a combination of those in quotation marks, separated by semicolons.
Additional arguments can be passed to the analyzers by setting the `CLANG_TIDY_ARGS`, `IWYU_ARGS` or `CPPCHECK_ARGS` variables.

### Sanitizers

Sanitizers can be enabled by configuring CMake with `-DUSE_SANITIZER=<Address | Memory | MemoryWithOrigins | Undefined | Thread | Leak | 'Address;Undefined'>`.

## Documentation

The documentation is automatically built and published whenever a GitHub Release is created.
To manually build documentation, call the following command:

```bash
cmake -S documentation -B build/doc
cmake --build build/doc --target GenerateDocs
# view the docs
open build/doc/doxygen/html/index.html
```

To build the documentation locally, you will need Doxygen, jinja2 and Pygments installed on your system.

## Architecture

SubzeroECS uses several key design patterns:

- **CRTP Systems**: Zero-overhead polymorphism for systems via Curiously Recurring Template Pattern
- **SoA Storage**: Components stored in Structure of Arrays for cache-friendly iteration
- **Set Intersection Views**: Efficient multi-component queries using sorted entity ID arrays
- **Type-Safe Collections**: Compile-time component type verification
- **Entity ID Recycling**: Free-list based entity ID reuse to prevent ID exhaustion

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

All contributions are accepted under the dual-license terms (AGPL-3.0 for open source, commercial license available).

## FAQ

### What makes SubzeroECS different from other ECS frameworks?

SubzeroECS prioritizes:
- Modern C++20 features for clean, type-safe APIs
- Minimal performance impact from entity composition fragmentation
- Header-mostly implementation for easy integration
- Clear, maintainable codebase suitable for learning and modification

### Can I use this for commercial projects?

Yes, SubzeroECS offers dual licensing:
- Open source projects can use the AGPL-3.0 license
- Commercial/proprietary projects should request a commercial license (see Licensing section below)

### How does performance compare to raw arrays?

For coherent access patterns (all entities with same components), SubzeroECS achieves ~52% of pure DOD SoA performance (193M vs 374M items/s at 100K entities) while providing significantly better developer ergonomics. The overhead comes from the view iteration and set intersection logic, which enables the flexible query system. At larger scales (10M entities), the gap narrows to ~72% (151M vs 210M items/s) as cache effects dominate and the N-way intersection algorithm's efficiency becomes more apparent.

### What's the entity capacity?

SubzeroECS uses 32-bit entity IDs with generation counters, supporting millions of concurrent entities with ID recycling.

## Related Projects

- [EnTT](https://github.com/skypjack/entt): High-performance ECS framework for C++17
- [flecs](https://github.com/SanderMertens/flecs): Fast and lightweight ECS with relationship support
- [EntityX](https://github.com/alecthomas/entityx): Fast, type-safe C++11 ECS

## Licensing

SubzeroECS is dual-licensed:

- **Open Source**: AGPL-3.0-only (see [LICENSE](LICENSE))
- **Commercial**: Proprietary license for closed-source embedding, private modifications, or SaaS operation without AGPL disclosure

**Why AGPL?**
Ensures improvements used in network-accessible deployments remain available to the community.

**How to Request Commercial Terms:**
Open a GitHub issue titled "Commercial License Request: [Your Organization Name]" with your intended use, scale, and timeline.

**Historical Versions:**
Releases prior to v1.0 were published under the Unlicense (public domain).

**Contributions:**
See [CONTRIBUTING.md](CONTRIBUTING.md) for dual-license inbound contribution terms.
