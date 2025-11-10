#include <benchmark/benchmark.h>
#include <random>

#include "oop_implementation.hpp"
#include "dod_implementation.hpp"
#include "ecs_implementation.hpp"

// ============================================================================
// Setup Helpers
// ============================================================================

// Random number generator for consistent initialization across all benchmarks
class RandomGenerator {
public:
    RandomGenerator(uint32_t seed = 42) : gen_(seed), dist_(-100.0f, 100.0f) {}
    
    float next() { return dist_(gen_); }

private:
    std::mt19937 gen_;
    std::uniform_real_distribution<float> dist_;
};

// ============================================================================
// OOP Benchmarks - Fragmented Object-Oriented Design
// Mixed entity types, pointer indirection, vtable lookups
// ============================================================================

static void BM_OOP_Fragmented_CreateEntities(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    
    // Benchmark entity creation
    for (auto _ : state) {
        state.PauseTiming();
        OOP_Fragmented::EntityManager manager;
        RandomGenerator rng;
        state.ResumeTiming();
        
        for (int64_t i = 0; i < entityCount; ++i) {
            manager.addEntity(rng.next(), rng.next(), rng.next(), rng.next(), static_cast<int>(i));
        }
        
        benchmark::DoNotOptimize(manager);
    }
    
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("OOP (Heap + Virtual + Mixed Types)");
}

static void BM_OOP_Fragmented_UpdatePositions(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    const float deltaTime = 1.0f / 60.0f; // 60 FPS
    
    OOP_Fragmented::EntityManager manager;
    RandomGenerator rng;
    
    // Create entities (not timed)
    for (int64_t i = 0; i < entityCount; ++i) {
        manager.addEntity(rng.next(), rng.next(), rng.next(), rng.next(), static_cast<int>(i));
    }
    
    // Benchmark the update loop
    for (auto _ : state) {
        manager.updateAll(deltaTime);
        benchmark::DoNotOptimize(manager);
    }
    
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("OOP (Heap + Virtual + Mixed Types)");
}

// ============================================================================
// DOD Benchmarks - Fragmented Data-Oriented Design
// Array of Structures with padding (cache-unfriendly)
// ============================================================================

static void BM_DOD_Fragmented_CreateEntities(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    
    // Benchmark entity creation
    for (auto _ : state) {
        state.PauseTiming();
        DOD_Fragmented::EntityData data;
        data.reserve(entityCount);
        RandomGenerator rng;
        state.ResumeTiming();
        
        for (int64_t i = 0; i < entityCount; ++i) {
            data.addEntity(rng.next(), rng.next(), rng.next(), rng.next());
        }
        
        benchmark::DoNotOptimize(data);
    }
    
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("DOD (AoS + Padding)");
}

static void BM_DOD_Fragmented_UpdatePositions(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    const float deltaTime = 1.0f / 60.0f; // 60 FPS
    
    DOD_Fragmented::EntityData data;
    data.reserve(entityCount);
    RandomGenerator rng;
    
    // Create entities (not timed)
    for (int64_t i = 0; i < entityCount; ++i) {
        data.addEntity(rng.next(), rng.next(), rng.next(), rng.next());
    }
    
    // Benchmark the update loop
    for (auto _ : state) {
        data.updateAll(deltaTime);
        benchmark::DoNotOptimize(data);
    }
    
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("DOD (AoS + Padding)");
}

// ============================================================================
// ECS Benchmarks - Entity Component System
// SubzeroECS with mixed entity compositions
// ============================================================================

static void BM_ECS_Fragmented_CreateEntities(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    
    // Benchmark entity creation
    for (auto _ : state) {
        state.PauseTiming();
        ECS_Fragmented::EntityWorld world;
        RandomGenerator rng;
        state.ResumeTiming();
        
        for (int64_t i = 0; i < entityCount; ++i) {
            world.addEntity(rng.next(), rng.next(), rng.next(), rng.next(), static_cast<int>(i));
        }
        
        benchmark::DoNotOptimize(world);
    }
    
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("SubzeroECS (Mixed Components)");
}

static void BM_ECS_Fragmented_UpdatePositions(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    const float deltaTime = 1.0f / 60.0f; // 60 FPS
    
    ECS_Fragmented::EntityWorld world;
    RandomGenerator rng;
    
    // Create entities (not timed)
    for (int64_t i = 0; i < entityCount; ++i) {
        world.addEntity(rng.next(), rng.next(), rng.next(), rng.next(), static_cast<int>(i));
    }
    
    // Benchmark the update loop
    for (auto _ : state) {
        world.updateAll(deltaTime);
        benchmark::DoNotOptimize(world);
    }
    
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("SubzeroECS (Mixed Components)");
}

// ============================================================================
// Benchmark Registration
// ============================================================================

// Register benchmarks in interleaved order
// These test FRAGMENTED patterns: pointer chasing, vtables, mixed types, padding

// Size: 10 entities - Entity Creation
BENCHMARK(BM_ECS_Fragmented_CreateEntities)->Arg(10)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_OOP_Fragmented_CreateEntities)->Arg(10)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_DOD_Fragmented_CreateEntities)->Arg(10)->Unit(benchmark::kMicrosecond);

// Size: 10 entities - Update
BENCHMARK(BM_ECS_Fragmented_UpdatePositions)->Arg(10)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_OOP_Fragmented_UpdatePositions)->Arg(10)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_DOD_Fragmented_UpdatePositions)->Arg(10)->Unit(benchmark::kMicrosecond);

// Size: 1K entities - Entity Creation
BENCHMARK(BM_ECS_Fragmented_CreateEntities)->Arg(1000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_OOP_Fragmented_CreateEntities)->Arg(1000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_DOD_Fragmented_CreateEntities)->Arg(1000)->Unit(benchmark::kMicrosecond);

// Size: 1K entities - Update
BENCHMARK(BM_ECS_Fragmented_UpdatePositions)->Arg(1000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_OOP_Fragmented_UpdatePositions)->Arg(1000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_DOD_Fragmented_UpdatePositions)->Arg(1000)->Unit(benchmark::kMicrosecond);

// Size: 100K entities - Entity Creation
BENCHMARK(BM_ECS_Fragmented_CreateEntities)->Arg(100000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_OOP_Fragmented_CreateEntities)->Arg(100000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_DOD_Fragmented_CreateEntities)->Arg(100000)->Unit(benchmark::kMicrosecond);

// Size: 100K entities - Update
BENCHMARK(BM_ECS_Fragmented_UpdatePositions)->Arg(100000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_OOP_Fragmented_UpdatePositions)->Arg(100000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_DOD_Fragmented_UpdatePositions)->Arg(100000)->Unit(benchmark::kMicrosecond);

// Size: 10M entities - Entity Creation
BENCHMARK(BM_ECS_Fragmented_CreateEntities)->Arg(10000000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_OOP_Fragmented_CreateEntities)->Arg(10000000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_DOD_Fragmented_CreateEntities)->Arg(10000000)->Unit(benchmark::kMicrosecond);

// Size: 10M entities - Update
BENCHMARK(BM_ECS_Fragmented_UpdatePositions)->Arg(10000000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_OOP_Fragmented_UpdatePositions)->Arg(10000000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_DOD_Fragmented_UpdatePositions)->Arg(10000000)->Unit(benchmark::kMicrosecond);

// Main function is provided by benchmark::benchmark_main
