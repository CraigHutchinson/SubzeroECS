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
// OOP Benchmarks
// ============================================================================

static void BM_OOP_UpdatePositions(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    const float deltaTime = 1.0f / 60.0f; // 60 FPS
    
    OOP::EntityManager manager;
    RandomGenerator rng;
    
    // Create entities
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
// DOD Benchmarks
// ============================================================================

static void BM_DOD_UpdatePositions(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    const float deltaTime = 1.0f / 60.0f; // 60 FPS
    
    DOD::EntityData data;
    data.reserve(entityCount);
    RandomGenerator rng;
    
    // Create entities
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
// ECS Benchmarks
// ============================================================================

static void BM_ECS_UpdatePositions(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    const float deltaTime = 1.0f / 60.0f; // 60 FPS
    
    ECS::EntityWorld world;
    RandomGenerator rng;
    
    // Create entities
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

// Test with varying entity counts: 10, 100, 1K, 10K, 100K
BENCHMARK(BM_OOP_UpdatePositions)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000)
    ->Arg(100000)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_DOD_UpdatePositions)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000)
    ->Arg(100000)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_ECS_UpdatePositions)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000)
    ->Arg(100000)
    ->Unit(benchmark::kMicrosecond);

// Main function is provided by benchmark::benchmark_main
