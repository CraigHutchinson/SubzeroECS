#pragma once

#include <vector>
#include <cmath>

namespace DOD_Fragmented {

// Fragmented DOD - Array of Structures (AoS) instead of Structure of Arrays (SoA)
// This is intentionally cache-unfriendly
struct Entity {
    float x, y;
    float vx, vy;
    // Padding and extra data to reduce cache efficiency
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

    // Update using Array of Structures - loads entire struct even though
    // we only need position and velocity
    void updateAll(float deltaTime) {
        for (auto& entity : entities_) {
            // Cache-unfriendly: reading large struct, padding causes cache misses
            entity.x += entity.vx * deltaTime;
            entity.y += entity.vy * deltaTime;
            entity.vy += 9.8f * deltaTime;
            entity.vx *= 0.99f;
            entity.vy *= 0.99f;
            
            if (entity.x < 0.0f) entity.x += 1000.0f;
            if (entity.x > 1000.0f) entity.x -= 1000.0f;
            if (entity.y < 0.0f) entity.y += 1000.0f;
            if (entity.y > 1000.0f) entity.y -= 1000.0f;
        }
    }

private:
    std::vector<Entity> entities_;
};

} // namespace DOD_Fragmented
