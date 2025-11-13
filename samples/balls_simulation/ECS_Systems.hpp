#pragma once

#include "SubzeroECS/World.hpp"
#include "SubzeroECS/System.hpp"
#include "Components.hpp"
#include "PhysicsCommon.hpp"

namespace BallsSim {

// ============================================================================
// Gravity System
// ============================================================================

// TODO: ECS Optimization - Instead of checking isAsleep flag, we could remove the
// Velocity component from sleeping entities. This would automatically exclude them
// from all systems that require Velocity, eliminating the need for runtime checks.
// When waking up, we'd add the Velocity component back with the stored values.
// This leverages ECS's component-based filtering for better cache efficiency and
// allows systems to operate only on truly active entities.

class GravitySystem : public SubzeroECS::System<GravitySystem, Velocity, Mass, SleepState> {
public:
    float deltaTime = 0.0f;
    float gravity = 980.0f;

    GravitySystem(SubzeroECS::World& world) 
        : SubzeroECS::System<GravitySystem, Velocity, Mass, SleepState>(world) {}

    void processEntity(Iterator it) {
        Velocity& vel = it.get<Velocity>();
        const SleepState& sleepState = it.get<SleepState>();
        
        if (!sleepState.isAsleep) {
            applyGravityToVelocity(vel.dy, gravity, deltaTime);
        }
    }
};

// ============================================================================
// Movement System
// ============================================================================

class MovementSystem : public SubzeroECS::System<MovementSystem, Position, Velocity, SleepState> {
public:
    float deltaTime = 0.0f;

    MovementSystem(SubzeroECS::World& world)
        : SubzeroECS::System<MovementSystem, Position, Velocity, SleepState>(world) {}

    void processEntity(Iterator it) {
        Position& pos = it.get<Position>();
        const Velocity& vel = it.get<Velocity>();
        const SleepState& sleepState = it.get<SleepState>();
        
        if (!sleepState.isAsleep) {
            updatePositionWithVelocity(pos.x, pos.y, vel.dx, vel.dy, deltaTime);
        }
    }
};

// ============================================================================
// Boundary Collision System
// ============================================================================

class BoundaryCollisionSystem : public SubzeroECS::System<BoundaryCollisionSystem, Position, Velocity, Radius, SleepState> {
public:
    PhysicsConfig config;

    BoundaryCollisionSystem(SubzeroECS::World& world)
        : SubzeroECS::System<BoundaryCollisionSystem, Position, Velocity, Radius, SleepState>(world) {}

    void processEntity(Iterator it) {
        Position& pos = it.get<Position>();
        Velocity& vel = it.get<Velocity>();
        const Radius& radius = it.get<Radius>();
        SleepState& sleepState = it.get<SleepState>();

        if (!sleepState.isAsleep) {
            handleWallCollision(pos.x, pos.y, vel.dx, vel.dy, radius.value, config);
            applyDamping(vel.dx, vel.dy, config.damping);
            updateSleepState(sleepState.isAsleep, sleepState.sleepTimer, 
                           vel.dx, vel.dy, 0.016f, config); // Approximate deltaTime
        }
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
        // Get all entities with Position, Velocity, Radius, Mass, SleepState
        SubzeroECS::View<Position, Velocity, Radius, Mass, SleepState> view(registry_);
        
        // O(n²) collision detection - brute force for simplicity
        // In a production system, you'd use spatial partitioning (quadtree, grid, etc.)
        std::vector<typename SubzeroECS::View<Position, Velocity, Radius, Mass, SleepState>::Iterator> entities;
        for (auto it = view.begin(); it != view.end(); ++it) {
            entities.push_back(it);
        }

        for (size_t i = 0; i < entities.size(); ++i) {
            // Skip if ball i is asleep
            if (entities[i].get<SleepState>().isAsleep) continue;
            
            for (size_t j = i + 1; j < entities.size(); ++j) {
                handleCollision(entities[i], entities[j]);
            }
        }
    }

private:
    SubzeroECS::CollectionRegistry& registry_;
    
    void handleCollision(typename SubzeroECS::View<Position, Velocity, Radius, Mass, SleepState>::Iterator it1, 
                        typename SubzeroECS::View<Position, Velocity, Radius, Mass, SleepState>::Iterator it2) {
        Position& pos1 = it1.get<Position>();
        Velocity& vel1 = it1.get<Velocity>();
        const Radius& rad1 = it1.get<Radius>();
        const Mass& mass1 = it1.get<Mass>();
        SleepState& sleep1 = it1.get<SleepState>();

        Position& pos2 = it2.get<Position>();
        Velocity& vel2 = it2.get<Velocity>();
        const Radius& rad2 = it2.get<Radius>();
        const Mass& mass2 = it2.get<Mass>();
        SleepState& sleep2 = it2.get<SleepState>();

        float dist, nx, ny;
        if (checkBallCollision(pos1.x, pos1.y, rad1.value,
                              pos2.x, pos2.y, rad2.value,
                              dist, nx, ny)) {
            // Wake up both balls on collision
            wakeUp(sleep1.isAsleep, sleep1.sleepTimer);
            wakeUp(sleep2.isAsleep, sleep2.sleepTimer);
            
            resolveBallCollision(
                pos1.x, pos1.y, vel1.dx, vel1.dy, mass1.value, rad1.value,
                pos2.x, pos2.y, vel2.dx, vel2.dy, mass2.value, rad2.value,
                dist, nx, ny, config.restitution
            );
        }
    }
};

} // namespace BallsSim
