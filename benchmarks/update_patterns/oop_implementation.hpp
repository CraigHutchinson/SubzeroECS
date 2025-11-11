#pragma once

#include <vector>
#include <memory>
#include "common.hpp"

// ============================================================================
// OOP Fragmented - Heterogeneous entities with different sizes
// ============================================================================
namespace OOP_Pattern {

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
        
#if 0 //TODO: This is a separate benchmark where we run more full on testing - UpdatePositions only for now
        Physics::updateRotationHealth(rotation_, health_, deltaTime);
#endif
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
        
#if 0 //TODO: This is a separate benchmark where we run more full on testing - UpdatePositions only for now
        Physics::updateRotationHealth(rotation_, health_, deltaTime);
        Physics::pulseScale(scale_, color_[0], color_[1], color_[2], deltaTime);
#endif
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
};

// Entity manager using unique_ptr - pointer indirection, heap fragmentation
class EntityManager {
public:
    void addEntity(float x, float y, float vx, float vy, EntityType entityType = EntityType::Small) {
        // Create different entity types to fragment memory
        switch (entityType) {
            case EntityType::Small:
                entities_.push_back(std::make_unique<SmallEntity>(x, y, vx, vy));
                break;
            case EntityType::Medium:
                entities_.push_back(std::make_unique<MediumEntity>(x, y, vx, vy));
                break;
            case EntityType::Large:
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

} // namespace OOP_Pattern
