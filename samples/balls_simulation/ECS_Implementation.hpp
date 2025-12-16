#pragma once

#include "SubzeroECS/World.hpp"
#include "Components.hpp"
#include "ECS_Systems.hpp"
#include <memory>

namespace BallsSim {

// ============================================================================
// Entity Component System (ECS) Implementation using SubzeroECS
// ============================================================================

class ECS_Implementation {
public:
    PhysicsConfig config;

    ECS_Implementation() {
        initialize();
    }

    void initialize() {
        world = std::make_unique<SubzeroECS::World>();
        
        gravitySystem = std::make_unique<GravitySystem>(*world);
        movementSystem = std::make_unique<MovementSystem>(*world);
        boundarySystem = std::make_unique<BoundaryCollisionSystem>(*world);
        collisionSystem = std::make_unique<BallCollisionSystem>(*world);
        
        gravitySystem->gravity = config.gravity;
        boundarySystem->config = config;
        collisionSystem->config = config;
    }

    void addBall(float x, float y, float dx, float dy, float radius, float mass, uint32_t color) {
        uint8_t r = (color >> 24) & 0xFF;
        uint8_t g = (color >> 16) & 0xFF;
        uint8_t b = (color >> 8) & 0xFF;
        uint8_t a = color & 0xFF;
        
        world->create(
            Position{x, y},
            Velocity{dx, dy},
            Radius{radius},
            Mass{mass},
            Color{r, g, b, a},
            SleepState{}
        );
    }

    void clear() {
        // Reinitialize to clear all entities
        world.reset();
        gravitySystem.reset();
        movementSystem.reset();
        boundarySystem.reset();
        collisionSystem.reset();
        initialize();
    }

    void update(float deltaTime) {
        gravitySystem->deltaTime = deltaTime;
        movementSystem->deltaTime = deltaTime;
        
        gravitySystem->update();
        movementSystem->update();
        boundarySystem->update();
        collisionSystem->update();
    }

    // Access to world for rendering
    SubzeroECS::World& getWorld() { return *world; }
    const SubzeroECS::World& getWorld() const { return *world; }

private:
    std::unique_ptr<SubzeroECS::World> world;
    std::unique_ptr<GravitySystem> gravitySystem;
    std::unique_ptr<MovementSystem> movementSystem;
    std::unique_ptr<BoundaryCollisionSystem> boundarySystem;
    std::unique_ptr<BallCollisionSystem> collisionSystem;
};

} // namespace BallsSim
