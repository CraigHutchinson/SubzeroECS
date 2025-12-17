#pragma once

#include "Components.hpp"
#include "PhysicsCommon.hpp"
#include <vector>

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
        // Apply gravity to all balls
        for (size_t i = 0; i < balls.count; ++i) {
            applyGravityToVelocity(balls.velocities_dy[i], config.gravity, deltaTime);
        }

        // Update positions
        for (size_t i = 0; i < balls.count; ++i) {
            updatePositionWithVelocity(balls.positions_x[i], balls.positions_y[i], 
                                      balls.velocities_dx[i], balls.velocities_dy[i], deltaTime);
        }

        // Handle boundary collisions
        for (size_t i = 0; i < balls.count; ++i) {
            handleWallCollision(balls.positions_x[i], balls.positions_y[i],
                               balls.velocities_dx[i], balls.velocities_dy[i],
                               balls.radii[i], config);
        }

        // Handle ball-to-ball collisions
        for (size_t i = 0; i < balls.count; ++i) {
            for (size_t j = i + 1; j < balls.count; ++j) {
                float dist, nx, ny;
                if (checkBallCollision(balls.positions_x[i], balls.positions_y[i], balls.radii[i],
                                      balls.positions_x[j], balls.positions_y[j], balls.radii[j],
                                      dist, nx, ny)) {
                    resolveBallCollision(
                        balls.positions_x[i], balls.positions_y[i], 
                        balls.velocities_dx[i], balls.velocities_dy[i], 
                        balls.masses[i], balls.radii[i],
                        balls.positions_x[j], balls.positions_y[j], 
                        balls.velocities_dx[j], balls.velocities_dy[j],
                        balls.masses[j], balls.radii[j],
                        dist, nx, ny, config.restitution
                    );
                }
            }
        }

        // Apply damping
        for (size_t i = 0; i < balls.count; ++i) {
            applyDamping(balls.velocities_dx[i], balls.velocities_dy[i], config.damping);
        }
    }

    size_t getEntityCount() const { return balls.count; }
};

} // namespace BallsSim
