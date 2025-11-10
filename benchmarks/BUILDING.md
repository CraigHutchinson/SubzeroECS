# Building and Running SubzeroECS Benchmarks

## Quick Start

### 1. Configure with Benchmarks Enabled

```bash
# On Linux/macOS
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSUBZEROECS_BUILD_BENCHMARKS=ON

# On Windows with Visual Studio
cmake -S . -B build -DSUBZEROECS_BUILD_BENCHMARKS=ON
```

### 2. Build the Benchmarks

```bash
# On Linux/macOS
cmake --build build --config Release

# On Windows
cmake --build build --config Release
```

### 3. Run the Position Update Benchmark

```bash
# On Linux/macOS
./build/benchmarks/position_update/position_update_benchmark

# On Windows
.\build\benchmarks\position_update\Release\position_update_benchmark.exe
```

## Understanding the Results

The benchmark compares three implementations:
- **OOP**: Traditional object-oriented approach with virtual dispatch
- **DOD**: Data-oriented design with Structure of Arrays
- **SubzeroECS**: Our ECS framework

Example output:
```
---------------------------------------------------------------------------
Benchmark                             Time             CPU   Iterations
---------------------------------------------------------------------------
BM_OOP_UpdatePositions/10          2.15 us         2.14 us       327680   OOP (Virtual Dispatch)
BM_DOD_UpdatePositions/10          0.95 us         0.95 us       747520   DOD (Structure of Arrays)
BM_ECS_UpdatePositions/10          1.12 us         1.12 us       627200   SubzeroECS
BM_OOP_UpdatePositions/100        21.50 us        21.40 us        32768   OOP (Virtual Dispatch)
BM_DOD_UpdatePositions/100         9.50 us         9.48 us        74752   DOD (Structure of Arrays)
BM_ECS_UpdatePositions/100        11.20 us        11.18 us        62720   SubzeroECS
```

**What the columns mean:**
- **Benchmark** - Test name and entity count
- **Time** - Wall clock time per iteration
- **CPU** - CPU time per iteration  
- **Iterations** - How many times the test ran (more = more confident results)
- **Label** - Implementation description

**Lower time is better!**

## Advanced Options

### Filter Specific Benchmarks

```bash
# Run only ECS benchmarks
./position_update_benchmark --benchmark_filter=ECS

# Run only with 1000 entities
./position_update_benchmark --benchmark_filter=/1000
```

### Output to JSON

```bash
./position_update_benchmark --benchmark_format=json > results.json
```

### Compare Results

```bash
# Run baseline
./position_update_benchmark --benchmark_out=baseline.json --benchmark_out_format=json

# After making changes, run again
./position_update_benchmark --benchmark_out=current.json --benchmark_out_format=json

# Compare (requires Google Benchmark compare.py script)
python3 compare.py benchmarks baseline.json current.json
```

## Benchmark Best Practices

1. **Always use Release builds** - Debug builds have vastly different performance
2. **Close other applications** - Minimize background processes during benchmarking
3. **Run multiple times** - Results can vary, look for consistency
4. **Consider thermal throttling** - CPU performance may decrease when hot
5. **Document your hardware** - Performance is hardware-specific

## Troubleshooting

### Benchmark not found

Make sure you enabled benchmarks during configuration:
```bash
cmake -S . -B build -DSUBZEROECS_BUILD_BENCHMARKS=ON
```

### Very slow results

Check that you built in Release mode:
```bash
cmake --build build --config Release
```

### Build errors

Google Benchmark is fetched automatically via CMake. If you have network issues:
1. Download Google Benchmark manually
2. Place it in `build/_deps/benchmark-src/`
3. Rebuild

## Adding New Benchmarks

See the [benchmarks README](./README.md) for guidelines on adding new benchmark tests.
