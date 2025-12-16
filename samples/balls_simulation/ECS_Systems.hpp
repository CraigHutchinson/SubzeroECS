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

        // Iterative collision resolution for stability in stacks
        for (int iteration = 0; iteration < config.collisionIterations; ++iteration) {
            for (size_t i = 0; i < entities.size(); ++i) {
                // Skip if ball i is asleep
                if (entities[i].get<SleepState>().isAsleep) continue;
                
                for (size_t j = i + 1; j < entities.size(); ++j) {
                    handleCollision(entities[i], entities[j]);
                }
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
            // Calculate impulse magnitude BEFORE resolving collision
            float impulseMagnitude = calculateCollisionImpulse(
                vel1.dx, vel1.dy, mass1.value, sleep1.isAsleep,
                vel2.dx, vel2.dy, mass2.value, sleep2.isAsleep,
                nx, ny, config.restitution
            );
            
            // Calculate wake-up threshold dynamically based on mass
            float avgMass = (mass1.value + mass2.value) * 0.5f;
            float wakeThreshold = config.getWakeUpImpulseThreshold(avgMass);
            
            // Wake up sleeping balls BEFORE resolving collision
            bool wakeup1 = shouldWakeUp(sleep1.isAsleep, impulseMagnitude, wakeThreshold);
            bool wakeup2 = shouldWakeUp(sleep2.isAsleep, impulseMagnitude, wakeThreshold);
            
            // If both are sleeping and colliding, wake at least one (the lighter one moves more easily)
            if (sleep1.isAsleep && sleep2.isAsleep) {
                if (mass1.value <= mass2.value) {
                    wakeup1 = true;
                } else {
                    wakeup2 = true;
                }
            }
            
            if (wakeup1) {
                wakeUp(sleep1.isAsleep, sleep1.sleepTimer);
            }
            if (wakeup2) {
                wakeUp(sleep2.isAsleep, sleep2.sleepTimer);
            }
            
            // Resolve collision - function handles all sleep state cases internally
            resolveBallCollision(
                pos1.x, pos1.y, vel1.dx, vel1.dy, mass1.value, rad1.value, sleep1.isAsleep,
                pos2.x, pos2.y, vel2.dx, vel2.dy, mass2.value, rad2.value, sleep2.isAsleep,
                dist, nx, ny, config.restitution
            );
        }
    }
};

} // namespace BallsSim
