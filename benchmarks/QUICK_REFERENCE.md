# SubzeroECS Benchmarks - Quick Reference

## Build Commands

```bash
# Configure (enable benchmarks)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSUBZEROECS_BUILD_BENCHMARKS=ON

# Build
cmake --build build --config Release

# Run position update benchmark
./build/benchmarks/position_update/position_update_benchmark        # Linux/macOS
.\build\benchmarks\position_update\Release\position_update_benchmark.exe  # Windows
```

## What's Being Tested

**Position Update Benchmark** - Updates entity positions with physics simulation

- **OOP**: Object-oriented with virtual dispatch
- **DOD**: Data-oriented with Structure of Arrays  
- **SubzeroECS**: ECS framework implementation

**Entity Scales**: 10, 100, 1K, 10K, 100K entities

## Benchmark Output

```
Benchmark                         Time        CPU    Iterations
BM_DOD_UpdatePositions/1000      95 us      95 us     74752    <- FASTEST
BM_ECS_UpdatePositions/1000     112 us     112 us     62720    <- SubzeroECS
BM_OOP_UpdatePositions/1000     215 us     215 us     32768    <- Slowest
```

**Lower time = Better performance**

## Common Tasks

### Run specific benchmark
```bash
./position_update_benchmark --benchmark_filter=ECS
```

### Run specific entity count
```bash
./position_update_benchmark --benchmark_filter=/1000
```

### Output to JSON
```bash
./position_update_benchmark --benchmark_out=results.json --benchmark_out_format=json
```

### Get help
```bash
./position_update_benchmark --help
```

## Benchmark Presets

### Release (Use for Performance Testing)
- `windows-x64-release-benchmark`
- `linux-x64-release-benchmark`
- `macos-release-benchmark`

### Debug (Use for Development/Debugging)
- `windows-x64-debug-benchmark`
- `linux-x64-debug-benchmark`
- `macos-debug-benchmark`

**⚠️ Always use Release for real benchmarks!** Debug is 10-20x slower.

## Expected Results

**Typical Performance Ranking:**
1. DOD (fastest) - Raw data-oriented design
2. SubzeroECS - Our ECS framework (~15-20% slower than DOD)
3. OOP (slowest) - Virtual dispatch overhead (~2x slower than DOD)

## Documentation

- `README.md` - Overview and references to other ECS benchmarks
- `BUILDING.md` - Detailed build and run instructions
- `PERFORMANCE_GUIDE.md` - How to interpret and analyze results
- `position_update/README.md` - Details about the position update test

## Reference Projects

- **[abeimler/ecs_benchmark](https://github.com/abeimler/ecs_benchmark)** - Comprehensive ECS framework comparisons
- **[EnTT](https://github.com/skypjack/entt)** - Fast header-only ECS library
- **[Flecs](https://github.com/SanderMertens/flecs)** - ECS with queries and relationships

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Benchmark not found | Add `-DSUBZEROECS_BUILD_BENCHMARKS=ON` to cmake |
| Very slow results | Use `-DCMAKE_BUILD_TYPE=Release` |
| Build errors | Check that Google Benchmark downloaded correctly |
| Inconsistent results | Close other apps, check CPU throttling |

## Performance Tips

✅ **Always use Release builds** - Debug is 10-20x slower
✅ **Close background apps** - Minimize interference  
✅ **Run multiple times** - Look for consistency
✅ **Compare ratios, not absolute times** - Hardware varies

❌ **Don't compare Debug builds** - Not representative
❌ **Don't compare across machines** - Different hardware
❌ **Don't benchmark when system is busy** - Unreliable results
