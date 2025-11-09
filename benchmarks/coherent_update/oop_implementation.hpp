#pragma once

#include <vector>
#include <memory>
#include <cmath>

namespace OOP {

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
        // Simple physics: position += velocity * deltaTime
        x_ += vx_ * deltaTime;
        y_ += vy_ * deltaTime;

        // Simple gravity effect
        vy_ += 9.8f * deltaTime;

        // Apply some damping to prevent infinite acceleration
        vx_ *= 0.99f;
        vy_ *= 0.99f;

        // Wrap around boundaries (0-1000 range)
        if (x_ < 0.0f) x_ += 1000.0f;
        if (x_ > 1000.0f) x_ -= 1000.0f;
        if (y_ < 0.0f) y_ += 1000.0f;
        if (y_ > 1000.0f) y_ -= 1000.0f;
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

    void clear() {
        entities_.clear();
    }

private:
    std::vector<std::unique_ptr<EntityBase>> entities_;
};

} // namespace OOP
