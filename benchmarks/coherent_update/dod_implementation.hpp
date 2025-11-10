#pragma once

#include <vector>
#include <cmath>

namespace DOD {

// Structure of Arrays (SoA) layout for cache-friendly access
struct EntityData {
    std::vector<float> positions_x;
    std::vector<float> positions_y;
    std::vector<float> velocities_x;
    std::vector<float> velocities_y;

    size_t size() const {
        return positions_x.size();
    }

    void reserve(size_t count) {
        positions_x.reserve(count);
        positions_y.reserve(count);
        velocities_x.reserve(count);
        velocities_y.reserve(count);
    }

    void clear() {
        positions_x.clear();
        positions_y.clear();
        velocities_x.clear();
        velocities_y.clear();
    }

    void addEntity(float x, float y, float vx, float vy) {
        positions_x.push_back(x);
        positions_y.push_back(y);
        velocities_x.push_back(vx);
        velocities_y.push_back(vy);
    }
};

// Update function that operates on the data arrays
inline void updatePositions(EntityData& data, float deltaTime) {
    const size_t count = data.size();
    
    // Process all data in contiguous memory - excellent cache locality
    for (size_t i = 0; i < count; ++i) {
        // Simple physics: position += velocity * deltaTime
        data.positions_x[i] += data.velocities_x[i] * deltaTime;
        data.positions_y[i] += data.velocities_y[i] * deltaTime;

        // Simple gravity effect
        data.velocities_y[i] += 9.8f * deltaTime;

        // Apply some damping
        data.velocities_x[i] *= 0.99f;
        data.velocities_y[i] *= 0.99f;

        // Wrap around boundaries (0-1000 range)
        if (data.positions_x[i] < 0.0f) data.positions_x[i] += 1000.0f;
        if (data.positions_x[i] > 1000.0f) data.positions_x[i] -= 1000.0f;
        if (data.positions_y[i] < 0.0f) data.positions_y[i] += 1000.0f;
        if (data.positions_y[i] > 1000.0f) data.positions_y[i] -= 1000.0f;
    }
}

} // namespace DOD
