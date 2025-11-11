#pragma once

#include "SubzeroECS/World.hpp"
#include "SubzeroECS/System.hpp"
#include "common.hpp"

// ============================================================================
// ECS Fragmented - Mixed entity compositions matching OOP/DOD types
// ============================================================================
namespace ECS_Pattern {

// Components
struct Position {
    float x = 0.0f;
    float y = 0.0f;
};

struct Velocity {
    float dx = 0.0f;
    float dy = 0.0f;
};

// Medium entity components
struct Health {
    float value = 100.0f;
};

struct Rotation {
    float angle = 0.0f;
};

struct Scale {
    float value = 1.0f;
};

// Large entity components
struct Color {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
};

struct Team {
    int id = 0;
};

struct Flags {
    int value = 0;
};

// Physics update system - processes all entities with Position and Velocity
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

// Rotation and Health update system - processes Medium and Large entities
class RotationHealthSystem : public SubzeroECS::System<RotationHealthSystem, Health, Rotation> {
public:
    float deltaTime = 0.0f;

    RotationHealthSystem(SubzeroECS::World& world)
        : SubzeroECS::System<RotationHealthSystem, Health, Rotation>(world) {}

    void processEntity(Iterator iEntity) {
        Health& health = iEntity.get<Health>();
        Rotation& rotation = iEntity.get<Rotation>();

        Physics::updateRotationHealth(rotation.angle, health.value, deltaTime);
    }
};

// Large entity extra processing system - only processes entities with Scale (and Color marker)
class ScalePulseSystem : public SubzeroECS::System<ScalePulseSystem, Scale, Color> {
public:
    float deltaTime = 0.0f;

    ScalePulseSystem(SubzeroECS::World& world)
        : SubzeroECS::System<ScalePulseSystem, Scale, Color>(world) {}

    void processEntity(Iterator iEntity) {
        Scale& scale = iEntity.get<Scale>();
        Color& color = iEntity.get<Color>();

        Physics::pulseScale(scale.value, color.r, color.g, color.b, deltaTime);
    }
};

// World wrapper for easier management
class EntityWorld {
public:
    EntityWorld() 
        : collections_(world_)
        , physicsSystem_(world_)
        , rotationHealthSystem_(world_)
        , scalePulseSystem_(world_)
    {}

    void addEntity(float x, float y, float vx, float vy, EntityType entityType = EntityType::Small) {
        switch (entityType) {
            case EntityType::Small:
                // Small entity - just Position and Velocity
                world_.create(Position{x, y}, Velocity{vx, vy});
                break;
            case EntityType::Medium:
                // Medium entity - adds Health, Rotation, Scale
                world_.create(Position{x, y}, Velocity{vx, vy}, Health{}, Rotation{}, Scale{});
                break;
            case EntityType::Large:
                // Large entity - adds Color, Team, Flags
                world_.create(Position{x, y}, Velocity{vx, vy}, Health{}, Rotation{}, Scale{}, Color{}, Team{}, Flags{});
                break;
        }
    }

    void updateAll(float deltaTime) {
        physicsSystem_.deltaTime = deltaTime;
        physicsSystem_.update();
        
#if 0 //TODO: This is a separate benchmark where we run more full on testing - UpdatePositions only for now
        rotationHealthSystem_.deltaTime = deltaTime;
        rotationHealthSystem_.update();
        
        scalePulseSystem_.deltaTime = deltaTime;
        scalePulseSystem_.update();
#endif
    }

    size_t count() const {
        const SubzeroECS::Collection<Position>& posCollection = 
            const_cast<SubzeroECS::World&>(world_).CollectionRegistry::get<Position>();
        return posCollection.size();
    }

private:
    SubzeroECS::World world_;
    SubzeroECS::Collection<Position, Velocity, Health, Rotation, Scale, Color, Team, Flags> collections_;
    PhysicsSystem physicsSystem_;
    RotationHealthSystem rotationHealthSystem_;
    ScalePulseSystem scalePulseSystem_;
};

} // namespace ECS_Pattern
