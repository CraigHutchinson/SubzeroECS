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

inline void updateRotationHealth(float& rotation, float& health, float deltaTime) {
    rotation += 0.1f * deltaTime;
    health -= 0.01f * deltaTime;
}

inline void pulseScale(float& scale, float& color_r, float& color_g, float& color_b, float deltaTime) {
    // Pulse the scale - simulate some additional work
    scale *= (1.0f + 0.001f * deltaTime);
    if (scale > 2.0f) scale = 1.0f;  // Reset if too large
    
    // Pulse color channels based on scale
    color_r = 0.5f + 0.5f * (scale - 1.0f);
    color_g = 0.5f + 0.5f * (2.0f - scale);
    color_b = 0.5f + 0.5f * ((scale - 1.0f) * (2.0f - scale));
}

} // namespace Physics
