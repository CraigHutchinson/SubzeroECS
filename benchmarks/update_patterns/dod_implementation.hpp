#pragma once

#include <vector>
#include "common.hpp"

// ============================================================================
// DOD Coherent - Structure of Arrays (SoA)
// ============================================================================
namespace DOD_Coherent {

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
        Physics::updatePosition(
            data.positions_x[i],
            data.positions_y[i],
            data.velocities_x[i],
            data.velocities_y[i],
            deltaTime
        );
    }
}

} // namespace DOD_Coherent

// ============================================================================
// DOD Fragmented - Array of Structures (AoS) with padding
// ============================================================================
namespace DOD_Fragmented {

// Fragmented AoS with padding to reduce cache efficiency
struct Entity {
    float x, y;
    float vx, vy;
    // Extra data and padding to reduce cache efficiency
    float health;
    float rotation;
    float scale;
    char padding[48]; // Force entities to not fit nicely in cache lines
};

class EntityData {
public:
    void reserve(size_t count) {
        entities_.reserve(count);
    }

    void addEntity(float x, float y, float vx, float vy) {
        entities_.push_back(Entity{
            x, y, vx, vy,
            100.0f,  // health
            0.0f,    // rotation
            1.0f     // scale
        });
    }

    size_t count() const {
        return entities_.size();
    }

    // Update using AoS - loads entire struct even though
    // we only need position and velocity
    void updateAll(float deltaTime) {
        for (auto& entity : entities_) {
            Physics::updatePosition(
                entity.x,
                entity.y,
                entity.vx,
                entity.vy,
                deltaTime
            );
        }
    }

private:
    std::vector<Entity> entities_;
};

} // namespace DOD_Fragmented
