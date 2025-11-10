#include <benchmark/benchmark.h>
#include "common.hpp"
#include "oop_implementation.hpp"
#include "dod_implementation.hpp"
#include "ecs_implementation.hpp"

// ============================================================================
// Benchmark Template Functions
// ============================================================================

// Helper macro to reduce code duplication
#define BENCHMARK_CREATE_ENTITIES(Pattern, Namespace, Label) \
static void BM_##Pattern##_CreateEntities(benchmark::State& state) { \
    const int64_t entityCount = state.range(0); \
    for (auto _ : state) { \
        state.PauseTiming(); \
        Namespace::EntityManager manager; \
        RandomGenerator rng; \
        state.ResumeTiming(); \
        for (int64_t i = 0; i < entityCount; ++i) { \
            manager.addEntity(rng.next(), rng.next(), rng.next(), rng.next()); \
        } \
        benchmark::DoNotOptimize(manager); \
    } \
    state.SetItemsProcessed(state.iterations() * entityCount); \
    state.SetLabel(Label); \
}

#define BENCHMARK_UPDATE_ENTITIES(Pattern, Namespace, Label) \
static void BM_##Pattern##_UpdatePositions(benchmark::State& state) { \
    const int64_t entityCount = state.range(0); \
    const float deltaTime = 1.0f / 60.0f; \
    Namespace::EntityManager manager; \
    RandomGenerator rng; \
    for (int64_t i = 0; i < entityCount; ++i) { \
        manager.addEntity(rng.next(), rng.next(), rng.next(), rng.next()); \
    } \
    for (auto _ : state) { \
        manager.updateAll(deltaTime); \
        benchmark::DoNotOptimize(manager); \
    } \
    state.SetItemsProcessed(state.iterations() * entityCount); \
    state.SetLabel(Label); \
}

// ============================================================================
// OOP Coherent Benchmarks
// ============================================================================

BENCHMARK_CREATE_ENTITIES(OOP_Coherent, OOP_Coherent, "OOP-Coherent")
BENCHMARK_UPDATE_ENTITIES(OOP_Coherent, OOP_Coherent, "OOP-Coherent")

// ============================================================================
// OOP Fragmented Benchmarks
// ============================================================================

static void BM_OOP_Fragmented_CreateEntities(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
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
    state.SetLabel("OOP-Fragmented");
}

static void BM_OOP_Fragmented_UpdatePositions(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    const float deltaTime = 1.0f / 60.0f;
    OOP_Fragmented::EntityManager manager;
    RandomGenerator rng;
    for (int64_t i = 0; i < entityCount; ++i) {
        manager.addEntity(rng.next(), rng.next(), rng.next(), rng.next(), static_cast<int>(i));
    }
    for (auto _ : state) {
        manager.updateAll(deltaTime);
        benchmark::DoNotOptimize(manager);
    }
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("OOP-Fragmented");
}

// ============================================================================
// DOD Coherent Benchmarks
// ============================================================================

static void BM_DOD_Coherent_CreateEntities(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    for (auto _ : state) {
        state.PauseTiming();
        DOD_Coherent::EntityData data;
        data.reserve(entityCount);
        RandomGenerator rng;
        state.ResumeTiming();
        for (int64_t i = 0; i < entityCount; ++i) {
            data.addEntity(rng.next(), rng.next(), rng.next(), rng.next());
        }
        benchmark::DoNotOptimize(data);
    }
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("DOD-Coherent");
}

static void BM_DOD_Coherent_UpdatePositions(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    const float deltaTime = 1.0f / 60.0f;
    DOD_Coherent::EntityData data;
    data.reserve(entityCount);
    RandomGenerator rng;
    for (int64_t i = 0; i < entityCount; ++i) {
        data.addEntity(rng.next(), rng.next(), rng.next(), rng.next());
    }
    for (auto _ : state) {
        DOD_Coherent::updatePositions(data, deltaTime);
        benchmark::DoNotOptimize(data);
    }
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("DOD-Coherent");
}

// ============================================================================
// DOD Fragmented Benchmarks
// ============================================================================

static void BM_DOD_Fragmented_CreateEntities(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
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
    state.SetLabel("DOD-Fragmented");
}

static void BM_DOD_Fragmented_UpdatePositions(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    const float deltaTime = 1.0f / 60.0f;
    DOD_Fragmented::EntityData data;
    data.reserve(entityCount);
    RandomGenerator rng;
    for (int64_t i = 0; i < entityCount; ++i) {
        data.addEntity(rng.next(), rng.next(), rng.next(), rng.next());
    }
    for (auto _ : state) {
        data.updateAll(deltaTime);
        benchmark::DoNotOptimize(data);
    }
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("DOD-Fragmented");
}

// ============================================================================
// ECS Coherent Benchmarks
// ============================================================================

static void BM_ECS_Coherent_CreateEntities(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    for (auto _ : state) {
        state.PauseTiming();
        ECS_Coherent::EntityWorld world;
        RandomGenerator rng;
        state.ResumeTiming();
        for (int64_t i = 0; i < entityCount; ++i) {
            world.addEntity(rng.next(), rng.next(), rng.next(), rng.next());
        }
        benchmark::DoNotOptimize(world);
    }
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("ECS-Coherent");
}

static void BM_ECS_Coherent_UpdatePositions(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    const float deltaTime = 1.0f / 60.0f;
    ECS_Coherent::EntityWorld world;
    RandomGenerator rng;
    for (int64_t i = 0; i < entityCount; ++i) {
        world.addEntity(rng.next(), rng.next(), rng.next(), rng.next());
    }
    for (auto _ : state) {
        world.updateAll(deltaTime);
        benchmark::DoNotOptimize(world);
    }
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("ECS-Coherent");
}

// ============================================================================
// ECS Fragmented Benchmarks
// ============================================================================

static void BM_ECS_Fragmented_CreateEntities(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
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
    state.SetLabel("ECS-Fragmented");
}

static void BM_ECS_Fragmented_UpdatePositions(benchmark::State& state) {
    const int64_t entityCount = state.range(0);
    const float deltaTime = 1.0f / 60.0f;
    ECS_Fragmented::EntityWorld world;
    RandomGenerator rng;
    for (int64_t i = 0; i < entityCount; ++i) {
        world.addEntity(rng.next(), rng.next(), rng.next(), rng.next(), static_cast<int>(i));
    }
    for (auto _ : state) {
        world.updateAll(deltaTime);
        benchmark::DoNotOptimize(world);
    }
    state.SetItemsProcessed(state.iterations() * entityCount);
    state.SetLabel("ECS-Fragmented");
}

// ============================================================================
// Benchmark Registration - Organized by Size, Pattern, and Operation
// ============================================================================

// Macro for registering all patterns at a given size
#define REGISTER_SIZE_BENCHMARKS(Size) \
    /* Size: Creation - Coherent */ \
    BENCHMARK(BM_ECS_Coherent_CreateEntities)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    BENCHMARK(BM_OOP_Coherent_CreateEntities)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    BENCHMARK(BM_DOD_Coherent_CreateEntities)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    /* Size: Creation - Fragmented */ \
    BENCHMARK(BM_ECS_Fragmented_CreateEntities)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    BENCHMARK(BM_OOP_Fragmented_CreateEntities)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    BENCHMARK(BM_DOD_Fragmented_CreateEntities)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    /* Size: Update - Coherent */ \
    BENCHMARK(BM_ECS_Coherent_UpdatePositions)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    BENCHMARK(BM_OOP_Coherent_UpdatePositions)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    BENCHMARK(BM_DOD_Coherent_UpdatePositions)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    /* Size: Update - Fragmented */ \
    BENCHMARK(BM_ECS_Fragmented_UpdatePositions)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    BENCHMARK(BM_OOP_Fragmented_UpdatePositions)->Arg(Size)->Unit(benchmark::kMicrosecond); \
    BENCHMARK(BM_DOD_Fragmented_UpdatePositions)->Arg(Size)->Unit(benchmark::kMicrosecond);

// Register benchmarks for each size
REGISTER_SIZE_BENCHMARKS(10)
REGISTER_SIZE_BENCHMARKS(1000)
REGISTER_SIZE_BENCHMARKS(100000)
REGISTER_SIZE_BENCHMARKS(10000000)

BENCHMARK_MAIN();
