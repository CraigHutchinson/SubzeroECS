# Performance Analysis Guide

## Understanding Benchmark Results

### Reading Google Benchmark Output

When you run a benchmark, you'll see output like this:

```
Run on (12 X 3130 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 256 KiB (x6)
  L3 Unified 12288 KiB (x1)
-----------------------------------------------------------------------
Benchmark                         Time             CPU   Iterations
-----------------------------------------------------------------------
BM_OOP_UpdatePositions/100     21.5 us         21.4 us        32768   OOP (Virtual Dispatch)
BM_DOD_UpdatePositions/100      9.5 us          9.5 us        74752   DOD (Structure of Arrays)
BM_ECS_UpdatePositions/100     11.2 us         11.2 us        62720   SubzeroECS
```

### Column Meanings

- **Benchmark**: Test name and parameter (e.g., `/100` = 100 entities)
- **Time**: Wall-clock time per iteration (what users experience)
- **CPU**: CPU time per iteration (actual processing time)
- **Iterations**: How many times the test ran (higher = more reliable)
- **Label**: Human-readable description

### Time Units

- **ns** (nanoseconds): 1/1,000,000,000 second - very fast operations
- **us** (microseconds): 1/1,000,000 second - typical for small batches
- **ms** (milliseconds): 1/1,000 second - larger operations
- **s** (seconds): Full second - very large operations

## Interpreting Performance Differences

### Example Analysis

```
BM_OOP_UpdatePositions/1000    215 us    <- OOP baseline
BM_DOD_UpdatePositions/1000     95 us    <- 2.26x faster than OOP
BM_ECS_UpdatePositions/1000    112 us    <- 1.92x faster than OOP
```

### What This Means

1. **DOD is fastest** (95 us)
   - Best cache locality from SoA layout
   - No virtual dispatch overhead
   - Compiler can auto-vectorize

2. **SubzeroECS is very competitive** (112 us)
   - Only 18% slower than raw DOD
   - Provides much more flexibility
   - Good abstraction/performance balance

3. **OOP is slowest** (215 us)
   - Virtual dispatch per entity
   - Pointer chasing hurts cache
   - 2x slower than DOD

## Why Cache Locality Matters

### Memory Access Patterns

**OOP (Poor Locality):**
```
Entity 1 -> [Virtual Ptr][Data] ... somewhere in memory
Entity 2 -> [Virtual Ptr][Data] ... somewhere else
Entity 3 -> [Virtual Ptr][Data] ... and somewhere else
```
- Each entity access is a cache miss
- Unpredictable memory jumps

**DOD/ECS (Good Locality):**
```
Positions: [x1][y1][x2][y2][x3][y3] ... contiguous
Velocities: [vx1][vy1][vx2][vy2] ... contiguous
```
- Sequential access patterns
- Entire cache lines utilized
- Prefetcher can predict next access

### Cache Line Example

Modern CPUs load 64 bytes at a time (cache line):
- **DOD**: One cache line = ~16 floats = 8 entities worth of data
- **OOP**: One cache line = 1-2 entity pointers + overhead

## Scalability Analysis

### Look for Performance Trends

```
Entities    OOP       DOD      ECS      Notes
--------  ------    ------    ------   -------------------------
10        2.1 us    0.9 us    1.1 us   OOP overhead dominates
100       21 us     9.5 us    11 us    2x difference persists
1000      215 us    95 us     112 us   DOD advantage grows
10000     2.1 ms    0.95 ms   1.1 ms   Pattern stable
100000    21 ms     9.5 ms    11 ms    Linear scaling
```

### What to Look For

1. **Linear Scaling**: Time should scale proportionally with entity count
2. **Constant Overhead**: Small entity counts may show setup costs
3. **Cache Effects**: Performance may drop at certain thresholds (L1→L2→L3→RAM)

## Hardware Impact

### CPU Cache Hierarchy

Performance changes drastically based on what fits in cache:

| Data Size | Fits In | Performance |
|-----------|---------|-------------|
| < 32 KB   | L1      | Excellent   |
| < 256 KB  | L2      | Very Good   |
| < 12 MB   | L3      | Good        |
| > 12 MB   | RAM     | Slower      |

### Example with 100K Entities

Position data: 100,000 × 8 bytes = 800 KB
- Fits in L3 cache → Good performance
- If 1M entities: 8 MB still in L3
- If 10M entities: 80 MB → Must use RAM → Slower

## Compiler Optimizations

### Impact of Build Type

```
                Release    Debug     Difference
-------------------------------------------------
DOD             9.5 us    125 us     13x slower
SubzeroECS     11.2 us    180 us     16x slower
OOP            21.5 us    350 us     16x slower
```

**Always benchmark in Release mode!**

### What Compilers Do

- **Inlining**: Removes function call overhead
- **Loop Unrolling**: Reduces branch instructions
- **Vectorization**: Uses SIMD instructions (4-8 operations at once)
- **Dead Code Elimination**: Removes unused code

## Variance and Confidence

### Iteration Count

Google Benchmark runs tests multiple times:
- **High iterations** (100K+): Very confident results
- **Low iterations** (100): Less reliable, more variance

### Expected Variance

Typical variance: ±2-5%
- If you see ±20%: System is under load or thermal throttling
- Run benchmarks when system is idle
- Close background applications

## Comparing Your Results

### Against Other Systems

Don't directly compare times across different machines:
- Different CPUs have different speeds
- Cache sizes vary
- Memory speeds differ

**Do compare ratios:**
- "DOD is 2.2x faster than OOP" is portable
- "DOD takes 95 us" is not portable

### Against Reference Benchmarks

Compare with abeimler/ecs_benchmark results:
- Look for similar patterns
- Verify your ECS performs comparably to EnTT, Flecs
- Investigate if results differ significantly

## Red Flags

### Watch Out For

1. **Non-linear scaling**: 10x entities != 10x time suggests issues
2. **Huge variance**: Results jumping around indicate interference
3. **Unexpected rankings**: If OOP beats DOD, something's wrong
4. **Debug-like performance**: Forgot Release build
5. **Thermal throttling**: Times increasing over the test run

## Taking Action

### If SubzeroECS is Slower Than Expected

1. **Check build type**: Must be Release
2. **Profile the code**: Find the hot spots
3. **Verify data layout**: Components should be contiguous
4. **Check cache misses**: Use `perf` on Linux or VTune
5. **Compare with DOD**: See what the gap is

### Optimization Priority

1. **Algorithm**: Wrong algorithm = orders of magnitude slower
2. **Data layout**: Poor layout = 2-10x slower
3. **Cache usage**: Cache misses = 2-5x slower  
4. **Micro-optimizations**: Usually < 20% improvement

Focus on the top of the list first!
