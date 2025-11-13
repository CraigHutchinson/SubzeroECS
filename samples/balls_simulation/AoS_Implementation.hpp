#pragma once

#include "Components.hpp"
#include "PhysicsCommon.hpp"
#include <vector>

namespace BallsSim {

// ============================================================================
// Array of Structures (AoS) Implementation
// ============================================================================

class AoS_Implementation {
public:
    struct Ball {
        Position position;
        Velocity velocity;
        float radius;
        float mass;
        uint32_t color; // RGBA packed
        bool isAsleep = false;
        float sleepTimer = 0.0f;
        
        // Convenience accessors for compatibility with existing code
        float& x() { return position.x; }
        float& y() { return position.y; }
        float& dx() { return velocity.dx; }
        float& dy() { return velocity.dy; }
        const float& x() const { return position.x; }
        const float& y() const { return position.y; }
        const float& dx() const { return velocity.dx; }
        const float& dy() const { return velocity.dy; }
    };

    std::vector<Ball> balls;
    PhysicsConfig config;

    void addBall(float x, float y, float dx, float dy, float radius, float mass, uint32_t color) {
        Ball ball;
        ball.position = {x, y};
        ball.velocity = {dx, dy};
        ball.radius = radius;
        ball.mass = mass;
        ball.color = color;
        ball.isAsleep = false;
        ball.sleepTimer = 0.0f;
        balls.push_back(ball);
    }

    void clear() {
        balls.clear();
    }

    void update(float deltaTime) {
        // Apply gravity to awake balls
        for (auto& ball : balls) {
            if (!ball.isAsleep) {
                applyGravityToVelocity(ball.velocity.dy, config.gravity, deltaTime);
            }
        }

        // Update positions
        for (auto& ball : balls) {
            if (!ball.isAsleep) {
                updatePositionWithVelocity(ball.position.x, ball.position.y, 
                                          ball.velocity.dx, ball.velocity.dy, deltaTime);
            }
        }

        // Handle boundary collisions
        for (auto& ball : balls) {
            if (!ball.isAsleep) {
                handleWallCollision(ball.position.x, ball.position.y, 
                                   ball.velocity.dx, ball.velocity.dy, 
                                   ball.radius, config);
            }
        }

        // Handle ball-to-ball collisions
        const size_t count = balls.size();
        for (size_t i = 0; i < count; ++i) {
            if (balls[i].isAsleep) continue;
            
            for (size_t j = i + 1; j < count; ++j) {
                float dist, nx, ny;
                auto& b1 = balls[i];
                auto& b2 = balls[j];
                
                if (checkBallCollision(b1.position.x, b1.position.y, b1.radius,
                                      b2.position.x, b2.position.y, b2.radius,
                                      dist, nx, ny)) {
                    // Wake up both balls
                    wakeUp(b1.isAsleep, b1.sleepTimer);
                    wakeUp(b2.isAsleep, b2.sleepTimer);
                    
                    resolveBallCollision(
                        b1.position.x, b1.position.y, b1.velocity.dx, b1.velocity.dy, 
                        b1.mass, b1.radius,
                        b2.position.x, b2.position.y, b2.velocity.dx, b2.velocity.dy,
                        b2.mass, b2.radius,
                        dist, nx, ny, config.restitution
                    );
                }
            }
        }

        // Apply damping and update sleep state
        for (auto& ball : balls) {
            if (!ball.isAsleep) {
                applyDamping(ball.velocity.dx, ball.velocity.dy, config.damping);
                updateSleepState(ball.isAsleep, ball.sleepTimer,
                               ball.velocity.dx, ball.velocity.dy,
                               deltaTime, config);
            }
        }
    }

    size_t getEntityCount() const { return balls.size(); }
};

} // namespace BallsSim
