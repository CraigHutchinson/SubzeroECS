#pragma once

#include <random>

// ============================================================================
// Shared Utilities
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
// Shared Physics Logic
// ============================================================================

namespace Physics {

inline void updatePosition(float& x, float& y, float& vx, float& vy, float deltaTime) {
    // Simple physics: position += velocity * deltaTime
    x += vx * deltaTime;
    y += vy * deltaTime;

    // Simple gravity effect
    vy += 9.8f * deltaTime;

    // Apply some damping
    vx *= 0.99f;
    vy *= 0.99f;

    // Wrap around boundaries (0-1000 range)
    if (x < 0.0f) x += 1000.0f;
    if (x > 1000.0f) x -= 1000.0f;
    if (y < 0.0f) y += 1000.0f;
    if (y > 1000.0f) y -= 1000.0f;
}

} // namespace Physics
