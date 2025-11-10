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
// DOD Fragmented - Array of SoA structures (one per entity type)
// ============================================================================
namespace DOD_Fragmented {

// Small entity SoA - just position and velocity
struct SmallEntities {
    std::vector<float> x, y;
    std::vector<float> vx, vy;

    void reserve(size_t count) {
        x.reserve(count);
        y.reserve(count);
        vx.reserve(count);
        vy.reserve(count);
    }

    void add(float px, float py, float pvx, float pvy) {
        x.push_back(px);
        y.push_back(py);
        vx.push_back(pvx);
        vy.push_back(pvy);
    }

    size_t size() const { return x.size(); }

    void updateAll(float deltaTime) {
        for (size_t i = 0; i < x.size(); ++i) {
            Physics::updatePosition(x[i], y[i], vx[i], vy[i], deltaTime);
        }
    }
};

// Medium entity SoA - adds health, rotation, scale
struct MediumEntities {
    std::vector<float> x, y;
    std::vector<float> vx, vy;
    std::vector<float> health;
    std::vector<float> rotation;
    std::vector<float> scale;

    void reserve(size_t count) {
        x.reserve(count);
        y.reserve(count);
        vx.reserve(count);
        vy.reserve(count);
        health.reserve(count);
        rotation.reserve(count);
        scale.reserve(count);
    }

    void add(float px, float py, float pvx, float pvy) {
        x.push_back(px);
        y.push_back(py);
        vx.push_back(pvx);
        vy.push_back(pvy);
        health.push_back(100.0f);
        rotation.push_back(0.0f);
        scale.push_back(1.0f);
    }

    size_t size() const { return x.size(); }

    void updateAll(float deltaTime) {
        for (size_t i = 0; i < x.size(); ++i) {
            Physics::updatePosition(x[i], y[i], vx[i], vy[i], deltaTime);
        }
        for (size_t i = 0; i < x.size(); ++i) {
            Physics::updateRotationHealth(rotation[i], health[i], deltaTime);
        }
    }
};

// Large entity SoA - adds color, team, flags
struct LargeEntities {
    std::vector<float> x, y;
    std::vector<float> vx, vy;
    std::vector<float> health;
    std::vector<float> rotation;
    std::vector<float> scale;
    std::vector<float> color_r, color_g, color_b, color_a;
    std::vector<int> team;
    std::vector<int> flags;

    void reserve(size_t count) {
        x.reserve(count);
        y.reserve(count);
        vx.reserve(count);
        vy.reserve(count);
        health.reserve(count);
        rotation.reserve(count);
        scale.reserve(count);
        color_r.reserve(count);
        color_g.reserve(count);
        color_b.reserve(count);
        color_a.reserve(count);
        team.reserve(count);
        flags.reserve(count);
    }

    void add(float px, float py, float pvx, float pvy) {
        x.push_back(px);
        y.push_back(py);
        vx.push_back(pvx);
        vy.push_back(pvy);
        health.push_back(100.0f);
        rotation.push_back(0.0f);
        scale.push_back(1.0f);
        color_r.push_back(1.0f);
        color_g.push_back(1.0f);
        color_b.push_back(1.0f);
        color_a.push_back(1.0f);
        team.push_back(0);
        flags.push_back(0);
    }

    size_t size() const { return x.size(); }

    void updateAll(float deltaTime) {
        for (size_t i = 0; i < x.size(); ++i) {
            Physics::updatePosition(x[i], y[i], vx[i], vy[i], deltaTime);
        }
        for (size_t i = 0; i < x.size(); ++i) {
            Physics::updateRotationHealth(rotation[i], health[i], deltaTime);
        }
        for (size_t i = 0; i < x.size(); ++i) {
            Physics::pulseScale(scale[i], color_r[i], color_g[i], color_b[i], deltaTime);
        }
    }
};

class EntityData {
public:
    void reserve(size_t count) {
        // Estimate roughly even distribution across types
        size_t per_type = count / 3;
        small_.reserve(per_type);
        medium_.reserve(per_type);
        large_.reserve(per_type);
    }

    void addEntity(float x, float y, float vx, float vy, int entityType = 0) {
        int type = entityType % 3;
        
        if (type == 0) {
            small_.add(x, y, vx, vy);
        } else if (type == 1) {
            medium_.add(x, y, vx, vy);
        } else {
            large_.add(x, y, vx, vy);
        }
    }

    size_t count() const {
        return small_.size() + medium_.size() + large_.size();
    }

    void updateAll(float deltaTime) {
        small_.updateAll(deltaTime);
        medium_.updateAll(deltaTime);
        large_.updateAll(deltaTime);
    }

private:
    SmallEntities small_;
    MediumEntities medium_;
    LargeEntities large_;
};

} // namespace DOD_Fragmented
