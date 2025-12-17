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
            : position{x, y}, velocity{dx, dy}, radius(radius), mass(mass), color(color) {}

        void applyGravity(float gravity, float deltaTime) {
            applyGravityToVelocity(velocity.dy, gravity, deltaTime);
        }

        void updatePosition(float deltaTime) {
            updatePositionWithVelocity(position.x, position.y, velocity.dx, velocity.dy, deltaTime);
        }

        void handleBoundaryCollision(const PhysicsConfig& config) {
            handleWallCollision(position.x, position.y, velocity.dx, velocity.dy, radius, config);
        }

        void applyDamping(const PhysicsConfig& config) {
            BallsSim::applyDamping(velocity.dx, velocity.dy, config.damping);
        }

        void collideWith(Ball& other, float deltaTime, float restitution) {
            float tCollision, dist, nx, ny;
            if (checkSweptCircleCollision(position.x, position.y, velocity.dx, velocity.dy, radius,
                                         other.position.x, other.position.y, other.velocity.dx, other.velocity.dy, other.radius,
                                         deltaTime,
                                         tCollision, dist, nx, ny)) {
                // Position both balls at collision point
                position.x += velocity.dx * deltaTime * tCollision;
                position.y += velocity.dy * deltaTime * tCollision;
                other.position.x += other.velocity.dx * deltaTime * tCollision;
                other.position.y += other.velocity.dy * deltaTime * tCollision;
                
                resolveBallCollision(
                    position.x, position.y, velocity.dx, velocity.dy, mass, radius,
                    other.position.x, other.position.y, other.velocity.dx, other.velocity.dy, 
                    other.mass, other.radius,
                    dist, nx, ny, restitution
                );
            }
        }

        Position position;
        Velocity velocity;
        float radius;
        float mass;
        uint32_t color; // RGBA packed
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
            for (size_t j = i + 1; j < count; ++j) {
                balls[i].collideWith(balls[j], deltaTime, config.restitution);
            }
        }

        // Apply damping
        for (auto& ball : balls) {
            ball.applyDamping(config);
        }
    }

    size_t getEntityCount() const { return balls.size(); }
};

} // namespace BallsSim
