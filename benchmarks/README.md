# SubzeroECS Benchmarks

This directory contains benchmark tests for the SubzeroECS framework, comparing performance against traditional programming approaches.

## Overview

The benchmarks in this repository aim to provide fair comparisons between different architectural patterns:

- **OOP (Object-Oriented Programming)**: Traditional class-based approach with virtual methods
- **DOD (Data-Oriented Design)**: Cache-friendly data layout with separate data arrays
- **SubzeroECS**: Our Entity-Component-System implementation

All three implementations share the same core update logic where possible to ensure fair comparisons.

## Benchmarks

### Position Update Benchmark

A stress test that updates positions of N entities based on velocity and simple physics rules. This benchmark tests:
- Iteration performance over large entity counts
- Cache efficiency of different data layouts
- Component access patterns

### Future Benchmarks

Planned benchmarks include:
- Entity creation/destruction
- Component addition/removal
- Complex system interactions
- Query performance

## Reference Implementations

For additional context and comparison with other ECS frameworks, see these excellent resources:

### Major ECS Benchmark Suites

- **[abeimler/ecs_benchmark](https://github.com/abeimler/ecs_benchmark)** - Comprehensive C++ ECS framework benchmarks
  - Compares: EnTT, Flecs, EntityX, Ginseng, mustache, OpenEcs, pico_ecs, gaia-ecs
  - Tests: Entity creation/destruction, component access, system updates, complex queries
  - Detailed performance graphs and analysis

### Notable ECS Frameworks

- **[EnTT](https://github.com/skypjack/entt)** - Fast and reliable entity-component system (header-only)
- **[Flecs](https://github.com/SanderMertens/flecs)** - Fast ECS with querying and relationship support
- **[EntityX](https://github.com/alecthomas/entityx)** - Fast, type-safe C++ Entity-Component system
- **[gaia-ecs](https://github.com/richardbiely/gaia-ecs)** - High-performance archetype-based ECS

### Additional Resources

- **[ECS FAQ](https://github.com/SanderMertens/ecs-faq)** - Comprehensive ECS questions and answers
- **[Awesome Entity Component System](https://github.com/jslee02/awesome-entity-component-system)** - Curated list of ECS resources

## Building and Running

### Quick Start with VS Code

If you're using Visual Studio Code with the CMake Tools extension:

1. Select a benchmark preset: `Ctrl+Shift+P` → `CMake: Select Configure Preset`
2. Choose: `Windows x64 Release + Benchmarks` (or your platform)
3. Build: Click "Build" in the status bar or press `F7`
4. Run: See [VSCODE.md](VSCODE.md) for detailed VS Code instructions

**Available Release Presets (Recommended):**
- `windows-x64-release-benchmark`
- `linux-x64-release-benchmark`
- `macos-release-benchmark`

**Debug Presets** (for development): `*-debug-benchmark` variants also available.

See [VSCODE.md](VSCODE.md) for complete VS Code workflow and tips.

### Build Benchmarks

```bash
# From the project root
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Run Benchmarks

```bash
# Run all benchmarks
./build/benchmarks/position_update/position_update_benchmark

# Or on Windows
.\build\benchmarks\position_update\Release\position_update_benchmark.exe
```

### Benchmark Output

Benchmarks use Google Benchmark library and output detailed performance metrics including:
- Time per iteration
- Items processed per second
- Statistical analysis (mean, median, stddev)

## Guidelines for Adding Benchmarks

When adding new benchmarks:

1. **Fair Comparisons**: Ensure all implementations use the same algorithms and data where possible
2. **Realistic Scenarios**: Model real-world use cases, not synthetic edge cases
3. **Document Assumptions**: Clearly state what each benchmark measures and any limitations
4. **Multiple Scales**: Test with varying entity counts (1, 100, 1K, 10K, 100K, 1M+)
5. **Reproducibility**: Use fixed seeds for random data, document system specs

## Performance Notes

Performance can vary significantly based on:
- Compiler optimizations (Release vs Debug)
- CPU cache sizes and architecture
- Memory allocator behavior
- Component data sizes and alignment

Always benchmark on your target hardware with your expected workload patterns.
