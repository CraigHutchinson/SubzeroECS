#pragma once

#include "SubzeroECS/World.hpp"
#include "SubzeroECS/System.hpp"
#include <cmath>

namespace ECS {

// Components - simple data structures
struct Position {
    float x = 0.0f;
    float y = 0.0f;
};

struct Velocity {
    float dx = 0.0f;
    float dy = 0.0f;
};

// Physics update system
class PhysicsSystem : public SubzeroECS::System<PhysicsSystem, Position, Velocity> {
public:
    float deltaTime = 0.0f;

    PhysicsSystem(SubzeroECS::World& world)
        : SubzeroECS::System<PhysicsSystem, Position, Velocity>(world) {}

    void processEntity(Iterator iEntity) {
        Position& pos = iEntity.get<Position>();
        Velocity& vel = iEntity.get<Velocity>();

        // Simple physics: position += velocity * deltaTime
        pos.x += vel.dx * deltaTime;
        pos.y += vel.dy * deltaTime;

        // Simple gravity effect
        vel.dy += 9.8f * deltaTime;

        // Apply some damping
        vel.dx *= 0.99f;
        vel.dy *= 0.99f;

        // Wrap around boundaries (0-1000 range)
        if (pos.x < 0.0f) pos.x += 1000.0f;
        if (pos.x > 1000.0f) pos.x -= 1000.0f;
        if (pos.y < 0.0f) pos.y += 1000.0f;
        if (pos.y > 1000.0f) pos.y -= 1000.0f;
    }
};

// World wrapper for easier management
class EntityWorld {
public:
    EntityWorld() 
        : collections_(world_)
        , physicsSystem_(world_) 
    {}

    void addEntity(float x, float y, float vx, float vy) {
        world_.create(Position{x, y}, Velocity{vx, vy});
    }

    void updateAll(float deltaTime) {
        physicsSystem_.deltaTime = deltaTime;
        physicsSystem_.update();
    }

    size_t count() const {
        // Count entities by iterating through the Position collection
        size_t entityCount = 0;
        SubzeroECS::Collection<Position>& posCollection = 
            const_cast<SubzeroECS::World&>(world_).CollectionRegistry::get<Position>();
        for (auto it = posCollection.begin(); it != posCollection.end(); ++it) {
            ++entityCount;
        }
        return entityCount;
    }

    void clear() {
        // We cannot reassign World (no copy assignment), so we'll need a different approach
        // For benchmarking purposes, we don't actually need to clear
        // If needed, could be implemented with a reset method or pointer indirection
    }

private:
    SubzeroECS::World world_;
    SubzeroECS::Collection<Position, Velocity> collections_;  // Register collections first
    PhysicsSystem physicsSystem_;
};

} // namespace ECS
