#pragma once

#include "Components.hpp"
#include "PhysicsCommon.hpp"
#include <vector>

namespace BallsSim {

// ============================================================================
// Object-Oriented Programming (OOP) Implementation
// ============================================================================

class OOP_Implementation {
public:
    class Ball {
    public:
        Ball(float x, float y, float dx, float dy, float radius, float mass, uint32_t color)
            : position{x, y}, velocity{dx, dy}, radius(radius), mass(mass), color(color), 
              isAsleep(false), sleepTimer(0.0f) {}

        void applyGravity(float gravity, float deltaTime) {
            if (!isAsleep) {
                applyGravityToVelocity(velocity.dy, gravity, deltaTime);
            }
        }

        void updatePosition(float deltaTime) {
            if (!isAsleep) {
                updatePositionWithVelocity(position.x, position.y, velocity.dx, velocity.dy, deltaTime);
            }
        }

        void handleBoundaryCollision(const PhysicsConfig& config) {
            if (!isAsleep) {
                handleWallCollision(position.x, position.y, velocity.dx, velocity.dy, radius, config);
            }
        }

        void applyDampingAndSleep(float deltaTime, const PhysicsConfig& config) {
            if (!isAsleep) {
                BallsSim::applyDamping(velocity.dx, velocity.dy, config.damping);
                updateSleepState(isAsleep, sleepTimer, velocity.dx, velocity.dy, deltaTime, config);
            }
        }

        void collideWith(Ball& other, float restitution, const PhysicsConfig& physicsConfig) {
            float dist, nx, ny;
            if (checkBallCollision(position.x, position.y, radius, 
                                  other.position.x, other.position.y, other.radius,
                                  dist, nx, ny)) {
                // Calculate impulse magnitude BEFORE resolving collision
                float impulseMagnitude = calculateCollisionImpulse(
                    velocity.dx, velocity.dy, mass, isAsleep,
                    other.velocity.dx, other.velocity.dy, other.mass, other.isAsleep,
                    nx, ny, restitution
                );
                
                // Wake up only if impulse is strong enough
                float avgMass = (mass + other.mass) * 0.5f;
                float wakeThreshold = physicsConfig.getWakeUpImpulseThreshold(avgMass);
                bool wakeup1 = shouldWakeUp(isAsleep, impulseMagnitude, wakeThreshold);
                bool wakeup2 = shouldWakeUp(other.isAsleep, impulseMagnitude, wakeThreshold);
                
                // If both are sleeping and colliding, wake at least one (the lighter one)
                if (isAsleep && other.isAsleep) {
                    if (mass <= other.mass) {
                        wakeup1 = true;
                    } else {
                        wakeup2 = true;
                    }
                }
                
                if (wakeup1) {
                    wakeUp(isAsleep, sleepTimer);
                }
                if (wakeup2) {
                    wakeUp(other.isAsleep, other.sleepTimer);
                }
                
                // Resolve collision - function handles all sleep state cases internally
                resolveBallCollision(
                    position.x, position.y, velocity.dx, velocity.dy, mass, radius, isAsleep,
                    other.position.x, other.position.y, other.velocity.dx, other.velocity.dy, 
                    other.mass, other.radius, other.isAsleep,
                    dist, nx, ny, restitution
                );
            }
        }

        Position position;
        Velocity velocity;
        float radius;
        float mass;
        uint32_t color; // RGBA packed
        bool isAsleep;
        float sleepTimer;
    };

    std::vector<Ball> balls;
    PhysicsConfig config;

    void addBall(float x, float y, float dx, float dy, float radius, float mass, uint32_t color) {
        balls.emplace_back(x, y, dx, dy, radius, mass, color);
    }

    void clear() {
        balls.clear();
    }

    void update(float deltaTime) {
        // Apply gravity
        for (auto& ball : balls) {
            ball.applyGravity(config.gravity, deltaTime);
        }

        // Update positions
        for (auto& ball : balls) {
            ball.updatePosition(deltaTime);
        }

        // Handle boundary collisions
        for (auto& ball : balls) {
            ball.handleBoundaryCollision(config);
        }

        // Handle ball-to-ball collisions
        const size_t count = balls.size();
        for (size_t i = 0; i < count; ++i) {
            if (balls[i].isAsleep) continue;
            
            for (size_t j = i + 1; j < count; ++j) {
                balls[i].collideWith(balls[j], config.restitution, config);
            }
        }

        // Apply damping and update sleep state
        for (auto& ball : balls) {
            ball.applyDampingAndSleep(deltaTime, config);
        }
    }

    size_t getEntityCount() const { return balls.size(); }
};

} // namespace BallsSim
