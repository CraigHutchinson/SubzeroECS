#pragma once

#include "Components.hpp"
#include <vector>
#include <cmath>

namespace BallsSim {

// ============================================================================
// Structure of Arrays (SoA) Implementation
// ============================================================================

class SoA_Implementation {
public:
    struct BallData {
        std::vector<float> positions_x;
        std::vector<float> positions_y;
        std::vector<float> velocities_dx;
        std::vector<float> velocities_dy;
        std::vector<float> radii;
        std::vector<float> masses;
        std::vector<uint32_t> colors; // RGBA packed
        size_t count = 0;
    };

    BallData balls;
    PhysicsConfig config;

    void addBall(float x, float y, float dx, float dy, float radius, float mass, uint32_t color) {
        balls.positions_x.push_back(x);
        balls.positions_y.push_back(y);
        balls.velocities_dx.push_back(dx);
        balls.velocities_dy.push_back(dy);
        balls.radii.push_back(radius);
        balls.masses.push_back(mass);
        balls.colors.push_back(color);
        balls.count++;
    }

    void clear() {
        balls.positions_x.clear();
        balls.positions_y.clear();
        balls.velocities_dx.clear();
        balls.velocities_dy.clear();
        balls.radii.clear();
        balls.masses.clear();
        balls.colors.clear();
        balls.count = 0;
    }

    void update(float deltaTime) {
        applyGravity(deltaTime);
        updatePositions(deltaTime);
        handleBoundaryCollisions();
        handleBallCollisions();
    }

private:
    void applyGravity(float deltaTime) {
        for (size_t i = 0; i < balls.count; ++i) {
            balls.velocities_dy[i] += config.gravity * deltaTime;
        }
    }

    void updatePositions(float deltaTime) {
        for (size_t i = 0; i < balls.count; ++i) {
            balls.positions_x[i] += balls.velocities_dx[i] * deltaTime;
            balls.positions_y[i] += balls.velocities_dy[i] * deltaTime;
        }
    }

    void handleBoundaryCollisions() {
        for (size_t i = 0; i < balls.count; ++i) {
            float& x = balls.positions_x[i];
            float& y = balls.positions_y[i];
            float& dx = balls.velocities_dx[i];
            float& dy = balls.velocities_dy[i];
            float radius = balls.radii[i];

            // Left/Right walls
            if (x - radius < 0.0f) {
                x = radius;
                dx = -dx * config.restitution * config.friction;
                dy *= config.friction;
            }
            else if (x + radius > config.boxWidth) {
                x = config.boxWidth - radius;
                dx = -dx * config.restitution * config.friction;
                dy *= config.friction;
            }

            // Top/Bottom walls
            if (y - radius < 0.0f) {
                y = radius;
                dy = -dy * config.restitution * config.friction;
                dx *= config.friction;
            }
            else if (y + radius > config.boxHeight) {
                y = config.boxHeight - radius;
                dy = -dy * config.restitution * config.friction;
                dx *= config.friction;
            }

            // Apply damping
            dx *= config.damping;
            dy *= config.damping;

            // Stop very slow movement
            if (std::abs(dx) < 0.1f) dx = 0.0f;
            if (std::abs(dy) < 0.1f) dy = 0.0f;
        }
    }

    void handleBallCollisions() {
        for (size_t i = 0; i < balls.count; ++i) {
            for (size_t j = i + 1; j < balls.count; ++j) {
                float dx = balls.positions_x[j] - balls.positions_x[i];
                float dy = balls.positions_y[j] - balls.positions_y[i];
                float dist = std::sqrt(dx * dx + dy * dy);
                float minDist = balls.radii[i] + balls.radii[j];

                if (dist < minDist && dist > 0.0f) {
                    // Normalize
                    float nx = dx / dist;
                    float ny = dy / dist;

                    // Separate
                    float overlap = minDist - dist;
                    float totalMass = balls.masses[i] + balls.masses[j];
                    float ratio1 = balls.masses[j] / totalMass;
                    float ratio2 = balls.masses[i] / totalMass;

                    balls.positions_x[i] -= nx * overlap * ratio1;
                    balls.positions_y[i] -= ny * overlap * ratio1;
                    balls.positions_x[j] += nx * overlap * ratio2;
                    balls.positions_y[j] += ny * overlap * ratio2;

                    // Calculate relative velocity
                    float dvx = balls.velocities_dx[j] - balls.velocities_dx[i];
                    float dvy = balls.velocities_dy[j] - balls.velocities_dy[i];
                    float dvn = dvx * nx + dvy * ny;

                    if (dvn < 0.0f) continue;

                    // Apply impulse
                    float impulse = -(1.0f + config.restitution) * dvn / totalMass;
                    balls.velocities_dx[i] -= impulse * balls.masses[j] * nx;
                    balls.velocities_dy[i] -= impulse * balls.masses[j] * ny;
                    balls.velocities_dx[j] += impulse * balls.masses[i] * nx;
                    balls.velocities_dy[j] += impulse * balls.masses[i] * ny;
                }
            }
        }
    }
};

// ============================================================================
// Array of Structures (AoS) Implementation
// ============================================================================

class AoS_Implementation {
public:
    struct Ball {
        float x, y;
        float dx, dy;
        float radius;
        float mass;
        uint32_t color;
    };

    std::vector<Ball> balls;
    PhysicsConfig config;

    void addBall(float x, float y, float dx, float dy, float radius, float mass, uint32_t color) {
        balls.push_back({x, y, dx, dy, radius, mass, color});
    }

    void clear() {
        balls.clear();
    }

    void update(float deltaTime) {
        applyGravity(deltaTime);
        updatePositions(deltaTime);
        handleBoundaryCollisions();
        handleBallCollisions();
    }

private:
    void applyGravity(float deltaTime) {
        for (auto& ball : balls) {
            ball.dy += config.gravity * deltaTime;
        }
    }

    void updatePositions(float deltaTime) {
        for (auto& ball : balls) {
            ball.x += ball.dx * deltaTime;
            ball.y += ball.dy * deltaTime;
        }
    }

    void handleBoundaryCollisions() {
        for (auto& ball : balls) {
            // Left/Right walls
            if (ball.x - ball.radius < 0.0f) {
                ball.x = ball.radius;
                ball.dx = -ball.dx * config.restitution * config.friction;
                ball.dy *= config.friction;
            }
            else if (ball.x + ball.radius > config.boxWidth) {
                ball.x = config.boxWidth - ball.radius;
                ball.dx = -ball.dx * config.restitution * config.friction;
                ball.dy *= config.friction;
            }

            // Top/Bottom walls
            if (ball.y - ball.radius < 0.0f) {
                ball.y = ball.radius;
                ball.dy = -ball.dy * config.restitution * config.friction;
                ball.dx *= config.friction;
            }
            else if (ball.y + ball.radius > config.boxHeight) {
                ball.y = config.boxHeight - ball.radius;
                ball.dy = -ball.dy * config.restitution * config.friction;
                ball.dx *= config.friction;
            }

            // Apply damping
            ball.dx *= config.damping;
            ball.dy *= config.damping;

            // Stop very slow movement
            if (std::abs(ball.dx) < 0.1f) ball.dx = 0.0f;
            if (std::abs(ball.dy) < 0.1f) ball.dy = 0.0f;
        }
    }

    void handleBallCollisions() {
        for (size_t i = 0; i < balls.size(); ++i) {
            for (size_t j = i + 1; j < balls.size(); ++j) {
                Ball& b1 = balls[i];
                Ball& b2 = balls[j];

                float dx = b2.x - b1.x;
                float dy = b2.y - b1.y;
                float dist = std::sqrt(dx * dx + dy * dy);
                float minDist = b1.radius + b2.radius;

                if (dist < minDist && dist > 0.0f) {
                    // Normalize
                    float nx = dx / dist;
                    float ny = dy / dist;

                    // Separate
                    float overlap = minDist - dist;
                    float totalMass = b1.mass + b2.mass;
                    float ratio1 = b2.mass / totalMass;
                    float ratio2 = b1.mass / totalMass;

                    b1.x -= nx * overlap * ratio1;
                    b1.y -= ny * overlap * ratio1;
                    b2.x += nx * overlap * ratio2;
                    b2.y += ny * overlap * ratio2;

                    // Calculate relative velocity
                    float dvx = b2.dx - b1.dx;
                    float dvy = b2.dy - b1.dy;
                    float dvn = dvx * nx + dvy * ny;

                    if (dvn < 0.0f) continue;

                    // Apply impulse
                    float impulse = -(1.0f + config.restitution) * dvn / totalMass;
                    b1.dx -= impulse * b2.mass * nx;
                    b1.dy -= impulse * b2.mass * ny;
                    b2.dx += impulse * b1.mass * nx;
                    b2.dy += impulse * b1.mass * ny;
                }
            }
        }
    }
};

// ============================================================================
// Object-Oriented (OOP) Implementation
// ============================================================================

class OOP_Implementation {
public:
    class Ball {
    public:
        float x, y;
        float dx, dy;
        float radius;
        float mass;
        uint32_t color;
        const PhysicsConfig* config;

        Ball(float x_, float y_, float dx_, float dy_, float radius_, float mass_, uint32_t color_, const PhysicsConfig* cfg)
            : x(x_), y(y_), dx(dx_), dy(dy_), radius(radius_), mass(mass_), color(color_), config(cfg) {}

        void applyGravity(float deltaTime) {
            dy += config->gravity * deltaTime;
        }

        void updatePosition(float deltaTime) {
            x += dx * deltaTime;
            y += dy * deltaTime;
        }

        void handleBoundaryCollision() {
            // Left/Right walls
            if (x - radius < 0.0f) {
                x = radius;
                dx = -dx * config->restitution * config->friction;
                dy *= config->friction;
            }
            else if (x + radius > config->boxWidth) {
                x = config->boxWidth - radius;
                dx = -dx * config->restitution * config->friction;
                dy *= config->friction;
            }

            // Top/Bottom walls
            if (y - radius < 0.0f) {
                y = radius;
                dy = -dy * config->restitution * config->friction;
                dx *= config->friction;
            }
            else if (y + radius > config->boxHeight) {
                y = config->boxHeight - radius;
                dy = -dy * config->restitution * config->friction;
                dx *= config->friction;
            }

            // Apply damping
            dx *= config->damping;
            dy *= config->damping;

            // Stop very slow movement
            if (std::abs(dx) < 0.1f) dx = 0.0f;
            if (std::abs(dy) < 0.1f) dy = 0.0f;
        }

        void collideWith(Ball& other) {
            float dx_val = other.x - x;
            float dy_val = other.y - y;
            float dist = std::sqrt(dx_val * dx_val + dy_val * dy_val);
            float minDist = radius + other.radius;

            if (dist < minDist && dist > 0.0f) {
                // Normalize
                float nx = dx_val / dist;
                float ny = dy_val / dist;

                // Separate
                float overlap = minDist - dist;
                float totalMass = mass + other.mass;
                float ratio1 = other.mass / totalMass;
                float ratio2 = mass / totalMass;

                x -= nx * overlap * ratio1;
                y -= ny * overlap * ratio1;
                other.x += nx * overlap * ratio2;
                other.y += ny * overlap * ratio2;

                // Calculate relative velocity
                float dvx = other.dx - dx;
                float dvy = other.dy - dy;
                float dvn = dvx * nx + dvy * ny;

                if (dvn < 0.0f) return;

                // Apply impulse
                float impulse = -(1.0f + config->restitution) * dvn / totalMass;
                dx -= impulse * other.mass * nx;
                dy -= impulse * other.mass * ny;
                other.dx += impulse * mass * nx;
                other.dy += impulse * mass * ny;
            }
        }
    };

    std::vector<Ball> balls;
    PhysicsConfig config;

    void addBall(float x, float y, float dx, float dy, float radius, float mass, uint32_t color) {
        balls.emplace_back(x, y, dx, dy, radius, mass, color, &config);
    }

    void clear() {
        balls.clear();
    }

    void update(float deltaTime) {
        // Apply gravity
        for (auto& ball : balls) {
            ball.applyGravity(deltaTime);
        }

        // Update positions
        for (auto& ball : balls) {
            ball.updatePosition(deltaTime);
        }

        // Handle boundary collisions
        for (auto& ball : balls) {
            ball.handleBoundaryCollision();
        }

        // Handle ball-to-ball collisions
        for (size_t i = 0; i < balls.size(); ++i) {
            for (size_t j = i + 1; j < balls.size(); ++j) {
                balls[i].collideWith(balls[j]);
            }
        }
    }
};

} // namespace BallsSim
