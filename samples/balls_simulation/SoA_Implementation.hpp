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
        std::vector<uint8_t> isAsleep;  // Using uint8_t instead of bool to avoid std::vector<bool> issues
        std::vector<float> sleepTimers;
        // Variance tracking for sleep detection
        std::vector<int> sampleCounts;
        std::vector<float> meanXs;
        std::vector<float> meanYs;
        std::vector<float> m2Xs;
        std::vector<float> m2Ys;
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
        balls.isAsleep.push_back(false);
        balls.sleepTimers.push_back(0.0f);
        balls.sampleCounts.push_back(0);
        balls.meanXs.push_back(0.0f);
        balls.meanYs.push_back(0.0f);
        balls.m2Xs.push_back(0.0f);
        balls.m2Ys.push_back(0.0f);
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
        balls.isAsleep.clear();
        balls.sleepTimers.clear();
        balls.sampleCounts.clear();
        balls.meanXs.clear();
        balls.meanYs.clear();
        balls.m2Xs.clear();
        balls.m2Ys.clear();
        balls.count = 0;
    }

    void update(float deltaTime) {
        // Apply gravity only to awake balls
        for (size_t i = 0; i < balls.count; ++i) {
            if (!balls.isAsleep[i]) {
                applyGravityToVelocity(balls.velocities_dy[i], config.gravity, deltaTime);
            }
        }

        // Update positions only for awake balls
        for (size_t i = 0; i < balls.count; ++i) {
            if (!balls.isAsleep[i]) {
                updatePositionWithVelocity(balls.positions_x[i], balls.positions_y[i], 
                                          balls.velocities_dx[i], balls.velocities_dy[i], deltaTime);
            }
        }

        // Handle boundary collisions
        for (size_t i = 0; i < balls.count; ++i) {
            if (!balls.isAsleep[i]) {
                handleWallCollision(balls.positions_x[i], balls.positions_y[i],
                                   balls.velocities_dx[i], balls.velocities_dy[i],
                                   balls.radii[i], config);
            }
        }

        // Handle ball-to-ball collisions
        for (size_t i = 0; i < balls.count; ++i) {
            if (balls.isAsleep[i]) continue;
            
            for (size_t j = i + 1; j < balls.count; ++j) {
                float dist, nx, ny;
                if (checkBallCollision(balls.positions_x[i], balls.positions_y[i], balls.radii[i],
                                      balls.positions_x[j], balls.positions_y[j], balls.radii[j],
                                      dist, nx, ny)) {
                    // Wake up both balls on collision
                    // Calculate impulse magnitude BEFORE resolving collision
                    float impulseMagnitude = calculateCollisionImpulse(
                        balls.velocities_dx[i], balls.velocities_dy[i], balls.masses[i], balls.isAsleep[i],
                        balls.velocities_dx[j], balls.velocities_dy[j], balls.masses[j], balls.isAsleep[j],
                        nx, ny, config.restitution
                    );
                    
                    // Wake up only if impulse is strong enough
                    float avgMass = (balls.masses[i] + balls.masses[j]) * 0.5f;
                    float wakeThreshold = config.getWakeUpImpulseThreshold(avgMass);
                    bool wakeup1 = shouldWakeUp(balls.isAsleep[i] != 0, impulseMagnitude, wakeThreshold);
                    bool wakeup2 = shouldWakeUp(balls.isAsleep[j] != 0, impulseMagnitude, wakeThreshold);
                    
                    // If both are sleeping and colliding, wake at least one (the lighter one)
                    if (balls.isAsleep[i] && balls.isAsleep[j]) {
                        if (balls.masses[i] <= balls.masses[j]) {
                            wakeup1 = true;
                        } else {
                            wakeup2 = true;
                        }
                    }
                    
                    if (wakeup1) {
                        bool asleep = balls.isAsleep[i] != 0;
                        wakeUpWithVariance(asleep, balls.sleepTimers[i], balls.sampleCounts[i],
                                          balls.meanXs[i], balls.meanYs[i], balls.m2Xs[i], balls.m2Ys[i]);
                        balls.isAsleep[i] = asleep ? 1 : 0;
                    }
                    if (wakeup2) {
                        bool asleep = balls.isAsleep[j] != 0;
                        wakeUpWithVariance(asleep, balls.sleepTimers[j], balls.sampleCounts[j],
                                          balls.meanXs[j], balls.meanYs[j], balls.m2Xs[j], balls.m2Ys[j]);
                        balls.isAsleep[j] = asleep ? 1 : 0;
                    }

                    // Resolve collision - function handles all sleep state cases internally
                    resolveBallCollision(
                        balls.positions_x[i], balls.positions_y[i], 
                        balls.velocities_dx[i], balls.velocities_dy[i], 
                        balls.masses[i], balls.radii[i], balls.isAsleep[i],
                        balls.positions_x[j], balls.positions_y[j], 
                        balls.velocities_dx[j], balls.velocities_dy[j],
                        balls.masses[j], balls.radii[j], balls.isAsleep[j],
                        dist, nx, ny, config.restitution
                    );
                }
            }
        }

        // Apply damping and update sleep state
        for (size_t i = 0; i < balls.count; ++i) {
            if (!balls.isAsleep[i]) {
                applyDamping(balls.velocities_dx[i], balls.velocities_dy[i], config.damping);
                bool a = balls.isAsleep[i];
                updateSleepStateWithVariance(a, balls.sleepTimers[i], balls.sampleCounts[i],
                                            balls.meanXs[i], balls.meanYs[i], 
                                            balls.m2Xs[i], balls.m2Ys[i],
                                            balls.positions_x[i], balls.positions_y[i],
                                            deltaTime, config);
                balls.isAsleep[i] = a;
            }
        }
    }

    size_t getEntityCount() const { return balls.count; }
};

} // namespace BallsSim
