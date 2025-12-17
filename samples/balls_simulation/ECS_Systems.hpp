#pragma once

#include "SubzeroECS/World.hpp"
#include "SubzeroECS/System.hpp"
#include "Components.hpp"
#include "PhysicsCommon.hpp"

namespace BallsSim {

// ============================================================================
// Gravity System
// ============================================================================

class GravitySystem : public SubzeroECS::System<GravitySystem, Velocity, Mass> {
public:
    float deltaTime = 0.0f;
    float gravity = 980.0f;

    GravitySystem(SubzeroECS::World& world) 
        : SubzeroECS::System<GravitySystem, Velocity, Mass>(world) {}

    void processEntity(Iterator it) {
        Velocity& vel = it.get<Velocity>();
        applyGravityToVelocity(vel.dy, gravity, deltaTime);
    }
};

// ============================================================================
// Movement System
// ============================================================================

class MovementSystem : public SubzeroECS::System<MovementSystem, Position, Velocity> {
public:
    float deltaTime = 0.0f;

    MovementSystem(SubzeroECS::World& world)
        : SubzeroECS::System<MovementSystem, Position, Velocity>(world) {}

    void processEntity(Iterator it) {
        Position& pos = it.get<Position>();
        const Velocity& vel = it.get<Velocity>();
        updatePositionWithVelocity(pos.x, pos.y, vel.dx, vel.dy, deltaTime);
    }
};

// ============================================================================
// Boundary Collision System
// ============================================================================

class BoundaryCollisionSystem : public SubzeroECS::System<BoundaryCollisionSystem, Position, Velocity, Radius> {
public:
    PhysicsConfig config;

    BoundaryCollisionSystem(SubzeroECS::World& world)
        : SubzeroECS::System<BoundaryCollisionSystem, Position, Velocity, Radius>(world) {}

    void processEntity(Iterator it) {
        Position& pos = it.get<Position>();
        Velocity& vel = it.get<Velocity>();
        const Radius& radius = it.get<Radius>();

        handleWallCollision(pos.x, pos.y, vel.dx, vel.dy, radius.value, config);
        applyDamping(vel.dx, vel.dy, config.damping);
    }
};

// ============================================================================
// Ball-to-Ball Collision System
// ============================================================================

class BallCollisionSystem {
public:
    PhysicsConfig config;
    
    BallCollisionSystem(SubzeroECS::CollectionRegistry& registry)
        : registry_(registry) {}

    void update() {
        // Get all entities with Position, Velocity, Radius, Mass
        SubzeroECS::View<Position, Velocity, Radius, Mass> view(registry_);
        
        // O(n²) collision detection - brute force for simplicity
        // In a production system, you'd use spatial partitioning (quadtree, grid, etc.)
        std::vector<typename SubzeroECS::View<Position, Velocity, Radius, Mass>::Iterator> entities;
        for (auto it = view.begin(); it != view.end(); ++it) {
            entities.push_back(it);
        }

        // Iterative collision resolution for stability in stacks
        for (int iteration = 0; iteration < config.collisionIterations; ++iteration) {
            for (size_t i = 0; i < entities.size(); ++i) {
                for (size_t j = i + 1; j < entities.size(); ++j) {
                    handleCollision(entities[i], entities[j]);
                }
            }
        }
    }

private:
    SubzeroECS::CollectionRegistry& registry_;
    
    void handleCollision(typename SubzeroECS::View<Position, Velocity, Radius, Mass>::Iterator it1, 
                        typename SubzeroECS::View<Position, Velocity, Radius, Mass>::Iterator it2) {
        Position& pos1 = it1.get<Position>();
        Velocity& vel1 = it1.get<Velocity>();
        const Radius& rad1 = it1.get<Radius>();
        const Mass& mass1 = it1.get<Mass>();

        Position& pos2 = it2.get<Position>();
        Velocity& vel2 = it2.get<Velocity>();
        const Radius& rad2 = it2.get<Radius>();
        const Mass& mass2 = it2.get<Mass>();

        float dist, nx, ny;
        if (checkBallCollision(pos1.x, pos1.y, rad1.value,
                              pos2.x, pos2.y, rad2.value,
                              dist, nx, ny)) {
            resolveBallCollision(
                pos1.x, pos1.y, vel1.dx, vel1.dy, mass1.value, rad1.value,
                pos2.x, pos2.y, vel2.dx, vel2.dy, mass2.value, rad2.value,
                dist, nx, ny, config.restitution
            );
        }
    }
};

} // namespace BallsSim
