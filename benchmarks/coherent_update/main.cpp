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
// OOP Benchmarks - Traditional Object-Oriented Design
// Each entity is an object with virtual methods (polymorphism)
// ============================================================================

static void BM_OOP_Coherent_CreateEntities(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    
    // Benchmark entity creation
    for (auto _ : state) {
        state.PauseTiming();
        OOP::EntityManager manager;
        RandomGenerator rng;
        state.ResumeTiming();
        
        for (int64_t i = 0; i < entityCount; ++i) {
            manager.addEntity(rng.next(), rng.next(), rng.next(), rng.next());
        }
        
        benchmark::DoNotOptimize(manager);
    }
    
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("OOP (Virtual Dispatch)");
}

static void BM_OOP_Coherent_UpdatePositions(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    const float deltaTime = 1.0f / 60.0f; // 60 FPS
    
    OOP::EntityManager manager;
    RandomGenerator rng;
    
    // Create entities (not timed)
    for (int64_t i = 0; i < entityCount; ++i) {
        manager.addEntity(rng.next(), rng.next(), rng.next(), rng.next());
    }
    
    // Benchmark the update loop
    for (auto _ : state) {
        manager.updateAll(deltaTime);
        benchmark::DoNotOptimize(manager);
    }
    
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("OOP (Virtual Dispatch)");
}

// ============================================================================
// DOD Benchmarks - Data-Oriented Design
// Components stored in contiguous arrays (Structure of Arrays)
// ============================================================================

static void BM_DOD_Coherent_CreateEntities(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    
    // Benchmark entity creation
    for (auto _ : state) {
        state.PauseTiming();
        DOD::EntityData data;
        data.reserve(entityCount);
        RandomGenerator rng;
        state.ResumeTiming();
        
        for (int64_t i = 0; i < entityCount; ++i) {
            data.addEntity(rng.next(), rng.next(), rng.next(), rng.next());
        }
        
        benchmark::DoNotOptimize(data);
    }
    
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("DOD (Structure of Arrays)");
}

static void BM_DOD_Coherent_UpdatePositions(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    const float deltaTime = 1.0f / 60.0f; // 60 FPS
    
    DOD::EntityData data;
    data.reserve(entityCount);
    RandomGenerator rng;
    
    // Create entities (not timed)
    for (int64_t i = 0; i < entityCount; ++i) {
        data.addEntity(rng.next(), rng.next(), rng.next(), rng.next());
    }
    
    // Benchmark the update loop
    for (auto _ : state) {
        DOD::updatePositions(data, deltaTime);
        benchmark::DoNotOptimize(data);
    }
    
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("DOD (Structure of Arrays)");
}

// ============================================================================
// ECS Benchmarks - Entity Component System
// SubzeroECS framework with Systems processing Components
// ============================================================================

static void BM_ECS_Coherent_CreateEntities(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    
    // Benchmark entity creation
    for (auto _ : state) {
        state.PauseTiming();
        ECS::EntityWorld world;
        RandomGenerator rng;
        state.ResumeTiming();
        
        for (int64_t i = 0; i < entityCount; ++i) {
            world.addEntity(rng.next(), rng.next(), rng.next(), rng.next());
        }
        
        benchmark::DoNotOptimize(world);
    }
    
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("SubzeroECS");
}

static void BM_ECS_Coherent_UpdatePositions(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    const float deltaTime = 1.0f / 60.0f; // 60 FPS
    
    ECS::EntityWorld world;
    RandomGenerator rng;
    
    // Create entities (not timed)
    for (int64_t i = 0; i < entityCount; ++i) {
        world.addEntity(rng.next(), rng.next(), rng.next(), rng.next());
    }
    
    // Benchmark the update loop
    for (auto _ : state) {
        world.updateAll(deltaTime);
        benchmark::DoNotOptimize(world);
    }
    
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("SubzeroECS");
}

// ============================================================================
// Benchmark Registration
// ============================================================================

// Register benchmarks in interleaved order so all implementations run at the same
// size before moving to the next size (ECS, OOP, DOD for each size)
// Each implementation uses its ideal pattern for data/function coherence

// Size: 10 entities - Entity Creation
BENCHMARK(BM_ECS_Coherent_CreateEntities)->Arg(10)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_OOP_Coherent_CreateEntities)->Arg(10)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_DOD_Coherent_CreateEntities)->Arg(10)->Unit(benchmark::kMicrosecond);

// Size: 10 entities - Update
BENCHMARK(BM_ECS_Coherent_UpdatePositions)->Arg(10)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_OOP_Coherent_UpdatePositions)->Arg(10)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_DOD_Coherent_UpdatePositions)->Arg(10)->Unit(benchmark::kMicrosecond);

// Size: 1K entities - Entity Creation
BENCHMARK(BM_ECS_Coherent_CreateEntities)->Arg(1000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_OOP_Coherent_CreateEntities)->Arg(1000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_DOD_Coherent_CreateEntities)->Arg(1000)->Unit(benchmark::kMicrosecond);

// Size: 1K entities - Update
BENCHMARK(BM_ECS_Coherent_UpdatePositions)->Arg(1000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_OOP_Coherent_UpdatePositions)->Arg(1000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_DOD_Coherent_UpdatePositions)->Arg(1000)->Unit(benchmark::kMicrosecond);

// Size: 100K entities - Entity Creation
BENCHMARK(BM_ECS_Coherent_CreateEntities)->Arg(100000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_OOP_Coherent_CreateEntities)->Arg(100000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_DOD_Coherent_CreateEntities)->Arg(100000)->Unit(benchmark::kMicrosecond);

// Size: 100K entities - Update
BENCHMARK(BM_ECS_Coherent_UpdatePositions)->Arg(100000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_OOP_Coherent_UpdatePositions)->Arg(100000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_DOD_Coherent_UpdatePositions)->Arg(100000)->Unit(benchmark::kMicrosecond);

// Size: 10M entities - Entity Creation
BENCHMARK(BM_ECS_Coherent_CreateEntities)->Arg(10000000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_OOP_Coherent_CreateEntities)->Arg(10000000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_DOD_Coherent_CreateEntities)->Arg(10000000)->Unit(benchmark::kMicrosecond);

// Size: 10M entities - Update
BENCHMARK(BM_ECS_Coherent_UpdatePositions)->Arg(10000000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_OOP_Coherent_UpdatePositions)->Arg(10000000)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_DOD_Coherent_UpdatePositions)->Arg(10000000)->Unit(benchmark::kMicrosecond);

// Main function is provided by benchmark::benchmark_main
