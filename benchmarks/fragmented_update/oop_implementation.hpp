#pragma once

#include <vector>
#include <memory>
#include <cmath>

namespace OOP_Fragmented {

// Base entity with virtual interface - forces vtable lookups
class EntityBase {
public:
    virtual ~EntityBase() = default;
    virtual void update(float deltaTime) = 0;
    virtual float getX() const = 0;
    virtual float getY() const = 0;
};

// Different entity types with varying sizes - fragments memory
// Small entity with just position and velocity
class SmallEntity : public EntityBase {
public:
    SmallEntity(float x, float y, float vx, float vy)
        : x_(x), y_(y), vx_(vx), vy_(vy) {}

    void update(float deltaTime) override {
        x_ += vx_ * deltaTime;
        y_ += vy_ * deltaTime;
        vy_ += 9.8f * deltaTime;
        vx_ *= 0.99f;
        vy_ *= 0.99f;
        
        if (x_ < 0.0f) x_ += 1000.0f;
        if (x_ > 1000.0f) x_ -= 1000.0f;
        if (y_ < 0.0f) y_ += 1000.0f;
        if (y_ > 1000.0f) y_ -= 1000.0f;
    }

    float getX() const override { return x_; }
    float getY() const override { return y_; }

private:
    float x_, y_, vx_, vy_;
};

// Medium entity with additional data - different size from SmallEntity
class MediumEntity : public EntityBase {
public:
    MediumEntity(float x, float y, float vx, float vy)
        : x_(x), y_(y), vx_(vx), vy_(vy)
        , health_(100.0f)
        , rotation_(0.0f)
        , scale_(1.0f) {}

    void update(float deltaTime) override {
        x_ += vx_ * deltaTime;
        y_ += vy_ * deltaTime;
        vy_ += 9.8f * deltaTime;
        vx_ *= 0.99f;
        vy_ *= 0.99f;
        
        rotation_ += 0.1f * deltaTime;
        health_ -= 0.01f * deltaTime;
        
        if (x_ < 0.0f) x_ += 1000.0f;
        if (x_ > 1000.0f) x_ -= 1000.0f;
        if (y_ < 0.0f) y_ += 1000.0f;
        if (y_ > 1000.0f) y_ -= 1000.0f;
    }

    float getX() const override { return x_; }
    float getY() const override { return y_; }

private:
    float x_, y_, vx_, vy_;
    float health_;
    float rotation_;
    float scale_;
};

// Large entity with lots of data - creates even more fragmentation
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
        x_ += vx_ * deltaTime;
        y_ += vy_ * deltaTime;
        vy_ += 9.8f * deltaTime;
        vx_ *= 0.99f;
        vy_ *= 0.99f;
        
        rotation_ += 0.1f * deltaTime;
        health_ -= 0.01f * deltaTime;
        
        if (x_ < 0.0f) x_ += 1000.0f;
        if (x_ > 1000.0f) x_ -= 1000.0f;
        if (y_ < 0.0f) y_ += 1000.0f;
        if (y_ > 1000.0f) y_ -= 1000.0f;
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
    char padding_[64]; // Extra padding to fragment memory even more
};

// Entity manager using unique_ptr - pointer indirection, heap fragmentation
class EntityManager {
public:
    void addEntity(float x, float y, float vx, float vy, int entityType = 0) {
        // Randomly create different entity types to fragment memory
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
        // Pointer chasing - each entity is allocated separately on heap
        // Virtual function calls - additional indirection through vtable
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
