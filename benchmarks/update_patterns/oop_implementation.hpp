#pragma once

#include <vector>
#include <memory>
#include "common.hpp"

// ============================================================================
// OOP Coherent - Homogeneous entities with virtual dispatch
// ============================================================================
namespace OOP_Coherent {

// Base class for entities using virtual dispatch
class EntityBase {
public:
    virtual ~EntityBase() = default;
    virtual void update(float deltaTime) = 0;
};

// Concrete entity with position and velocity
class MovingEntity : public EntityBase {
public:
    MovingEntity(float x, float y, float vx, float vy)
        : x_(x), y_(y), vx_(vx), vy_(vy) {}

    void update(float deltaTime) override {
        Physics::updatePosition(x_, y_, vx_, vy_, deltaTime);
    }

    float getX() const { return x_; }
    float getY() const { return y_; }

private:
    float x_, y_;   // Position
    float vx_, vy_; // Velocity
};

// Manager class that owns all entities
class EntityManager {
public:
    void addEntity(float x, float y, float vx, float vy) {
        entities_.push_back(std::make_unique<MovingEntity>(x, y, vx, vy));
    }

    void updateAll(float deltaTime) {
        for (auto& entity : entities_) {
            entity->update(deltaTime);
        }
    }

    size_t count() const {
        return entities_.size();
    }

private:
    std::vector<std::unique_ptr<EntityBase>> entities_;
};

} // namespace OOP_Coherent

// ============================================================================
// OOP Fragmented - Heterogeneous entities with different sizes
// ============================================================================
namespace OOP_Fragmented {

// Base entity with virtual interface - forces vtable lookups
class EntityBase {
public:
    virtual ~EntityBase() = default;
    virtual void update(float deltaTime) = 0;
    virtual float getX() const = 0;
    virtual float getY() const = 0;
};

// Small entity with just position and velocity
class SmallEntity : public EntityBase {
public:
    SmallEntity(float x, float y, float vx, float vy)
        : x_(x), y_(y), vx_(vx), vy_(vy) {}

    void update(float deltaTime) override {
        Physics::updatePosition(x_, y_, vx_, vy_, deltaTime);
    }

    float getX() const override { return x_; }
    float getY() const override { return y_; }

private:
    float x_, y_, vx_, vy_;
};

// Medium entity with additional data
class MediumEntity : public EntityBase {
public:
    MediumEntity(float x, float y, float vx, float vy)
        : x_(x), y_(y), vx_(vx), vy_(vy)
        , health_(100.0f)
        , rotation_(0.0f)
        , scale_(1.0f) {}

    void update(float deltaTime) override {
        Physics::updatePosition(x_, y_, vx_, vy_, deltaTime);
        Physics::updateRotationHealth(rotation_, health_, deltaTime);
    }

    float getX() const override { return x_; }
    float getY() const override { return y_; }

private:
    float x_, y_, vx_, vy_;
    float health_;
    float rotation_;
    float scale_;
};

// Large entity with lots of data
class LargeEntity : public EntityBase {
public:
    LargeEntity(float x, float y, float vx, float vy)
        : x_(x), y_(y), vx_(vx), vy_(vy)
        , health_(100.0f)
        , rotation_(0.0f)
        , scale_(1.0f)
        , color_{1.0f, 1.0f, 1.0f, 1.0f}
        , team_(0)
        , flags_(0) {}

    void update(float deltaTime) override {
        Physics::updatePosition(x_, y_, vx_, vy_, deltaTime);
        Physics::updateRotationHealth(rotation_, health_, deltaTime);
        Physics::pulseScale(scale_, deltaTime);
    }

    float getX() const override { return x_; }
    float getY() const override { return y_; }

private:
    float x_, y_, vx_, vy_;
    float health_;
    float rotation_;
    float scale_;
    float color_[4];
    int team_;
    int flags_;
    char padding_[64]; // Extra padding to fragment memory
};

// Entity manager using unique_ptr - pointer indirection, heap fragmentation
class EntityManager {
public:
    void addEntity(float x, float y, float vx, float vy, int entityType = 0) {
        // Create different entity types to fragment memory
        switch (entityType % 3) {
            case 0:
                entities_.push_back(std::make_unique<SmallEntity>(x, y, vx, vy));
                break;
            case 1:
                entities_.push_back(std::make_unique<MediumEntity>(x, y, vx, vy));
                break;
            case 2:
                entities_.push_back(std::make_unique<LargeEntity>(x, y, vx, vy));
                break;
        }
    }

    void updateAll(float deltaTime) {
        for (auto& entity : entities_) {
            entity->update(deltaTime);
        }
    }

    size_t count() const {
        return entities_.size();
    }

private:
    std::vector<std::unique_ptr<EntityBase>> entities_;
};

} // namespace OOP_Fragmented
