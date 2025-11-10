#pragma once

#include "SubzeroECS/World.hpp"
#include "SubzeroECS/System.hpp"
#include "common.hpp"

// ============================================================================
// ECS Coherent - All entities have the same components
// ============================================================================
namespace ECS_Coherent {

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

        Physics::updatePosition(pos.x, pos.y, vel.dx, vel.dy, deltaTime);
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
        const SubzeroECS::Collection<Position>& posCollection = 
            const_cast<SubzeroECS::World&>(world_).CollectionRegistry::get<Position>();
        return posCollection.size();
    }

private:
    SubzeroECS::World world_;
    SubzeroECS::Collection<Position, Velocity> collections_;
    PhysicsSystem physicsSystem_;
};

} // namespace ECS_Coherent

// ============================================================================
// ECS Fragmented - Mixed entity compositions
// ============================================================================
namespace ECS_Fragmented {

// Components
struct Position {
    float x = 0.0f;
    float y = 0.0f;
};

struct Velocity {
    float dx = 0.0f;
    float dy = 0.0f;
};

// Extra component to create mixed entity compositions (fragmentation)
struct ExtraData {
    float health = 100.0f;
    float rotation = 0.0f;
    char padding[56]; // Make it larger to create some fragmentation
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

        Physics::updatePosition(pos.x, pos.y, vel.dx, vel.dy, deltaTime);
    }
};

// World wrapper for easier management
class EntityWorld {
public:
    EntityWorld() 
        : collections_(world_)
        , physicsSystem_(world_) 
    {}

    void addEntity(float x, float y, float vx, float vy, int entityType = 0) {
        // Mix entities - some have ExtraData, some don't
        // This creates fragmentation in entity composition
        if (entityType % 3 == 0) {
            world_.create(Position{x, y}, Velocity{vx, vy}, ExtraData{});
        } else {
            world_.create(Position{x, y}, Velocity{vx, vy});
        }
    }

    void updateAll(float deltaTime) {
        physicsSystem_.deltaTime = deltaTime;
        physicsSystem_.update();
    }

    size_t count() const {
        const SubzeroECS::Collection<Position>& posCollection = 
            const_cast<SubzeroECS::World&>(world_).CollectionRegistry::get<Position>();
        return posCollection.size();
    }

private:
    SubzeroECS::World world_;
    SubzeroECS::Collection<Position, Velocity, ExtraData> collections_;
    PhysicsSystem physicsSystem_;
};

} // namespace ECS_Fragmented
