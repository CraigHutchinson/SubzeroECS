#include <benchmark/benchmark.h>
#include "common.hpp"
#include "oop_implementation.hpp"
#include "dod_implementation.hpp"
#include "ecs_implementation.hpp"

// ============================================================================
// Generic Benchmarks using templates
// ============================================================================

template<typename WorldType>
static void BM_CreateEntities(benchmark::State& state, DistributionPattern pattern) {
    const int64_t entityCount = state.range(0);
    
    for (auto _ : state) {
        state.PauseTiming();
        WorldType world;
        if constexpr (requires { world.reserve(entityCount); }) {
            world.reserve(entityCount);
        }
        RandomGenerator rng;
        state.ResumeTiming();
        for (int64_t i = 0; i < entityCount; ++i) {
            world.addEntity(rng.next(), rng.next(), rng.next(), rng.next(), getEntityType(i, pattern));
        }
        benchmark::DoNotOptimize(world);
    }
    state.SetItemsProcessed(state.iterations() * entityCount);
}

template<typename WorldType>
static void BM_UpdateEntities(benchmark::State& state, DistributionPattern pattern) {
    const int64_t entityCount = state.range(0);
    const float deltaTime = 1.0f / 60.0f;
    
    WorldType world;
    if constexpr (requires { world.reserve(entityCount); }) {
        world.reserve(entityCount);
    }
    RandomGenerator rng;
    for (int64_t i = 0; i < entityCount; ++i) {
        world.addEntity(rng.next(), rng.next(), rng.next(), rng.next(), getEntityType(i, pattern));
    }
    for (auto _ : state) {
        world.updateAll(deltaTime);
        benchmark::DoNotOptimize(world);
    }
    state.SetItemsProcessed(state.iterations() * entityCount);
}

// ============================================================================
// Benchmark Registration - Using BENCHMARK_CAPTURE for both type and pattern
// ============================================================================

#define REGISTER_SIZE_BENCHMARKS(Size) \
    /* Size: Creation - Coherent */ \
    BENCHMARK_CAPTURE(BM_CreateEntities<ECS_Pattern::EntityWorld>, ECS_Coherent, DistributionPattern::Coherent)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    BENCHMARK_CAPTURE(BM_CreateEntities<OOP_Pattern::EntityManager>, OOP_Coherent, DistributionPattern::Coherent)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    BENCHMARK_CAPTURE(BM_CreateEntities<DOD_Pattern::EntityData>, DOD_Coherent, DistributionPattern::Coherent)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    /* Size: Creation - Fragmented */ \
    BENCHMARK_CAPTURE(BM_CreateEntities<ECS_Pattern::EntityWorld>, ECS_Fragmented, DistributionPattern::Fragmented)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    BENCHMARK_CAPTURE(BM_CreateEntities<OOP_Pattern::EntityManager>, OOP_Fragmented, DistributionPattern::Fragmented)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    BENCHMARK_CAPTURE(BM_CreateEntities<DOD_Pattern::EntityData>, DOD_Fragmented, DistributionPattern::Fragmented)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    /* Size: Update - Coherent */ \
    BENCHMARK_CAPTURE(BM_UpdateEntities<ECS_Pattern::EntityWorld>, ECS_Coherent, DistributionPattern::Coherent)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    BENCHMARK_CAPTURE(BM_UpdateEntities<OOP_Pattern::EntityManager>, OOP_Coherent, DistributionPattern::Coherent)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    BENCHMARK_CAPTURE(BM_UpdateEntities<DOD_Pattern::EntityData>, DOD_Coherent, DistributionPattern::Coherent)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    /* Size: Update - Fragmented */ \
    BENCHMARK_CAPTURE(BM_UpdateEntities<ECS_Pattern::EntityWorld>, ECS_Fragmented, DistributionPattern::Fragmented)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    BENCHMARK_CAPTURE(BM_UpdateEntities<OOP_Pattern::EntityManager>, OOP_Fragmented, DistributionPattern::Fragmented)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    BENCHMARK_CAPTURE(BM_UpdateEntities<DOD_Pattern::EntityData>, DOD_Fragmented, DistributionPattern::Fragmented)->Arg(Size)->Unit(benchmark::kMicrosecond);

// Register benchmarks for each size
REGISTER_SIZE_BENCHMARKS(10)
REGISTER_SIZE_BENCHMARKS(1000)
REGISTER_SIZE_BENCHMARKS(100000)
REGISTER_SIZE_BENCHMARKS(10000000)

BENCHMARK_MAIN();
