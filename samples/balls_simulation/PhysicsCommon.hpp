#pragma once

#include "Components.hpp"
#include <cmath>

namespace BallsSim {

// ============================================================================
// Common Physics Functions
// ============================================================================

inline void applyGravityToVelocity(float& velocityY, float gravity, float deltaTime) {
    velocityY += gravity * deltaTime;
}

inline void updatePositionWithVelocity(float& posX, float& posY, float velX, float velY, float deltaTime) {
    posX += velX * deltaTime;
    posY += velY * deltaTime;
}

inline void applyDamping(float& velocityX, float& velocityY, float damping) {
    velocityX *= damping;
    velocityY *= damping;
    
    // Stop very slow movement
    if (std::abs(velocityX) < 0.1f) velocityX = 0.0f;
    if (std::abs(velocityY) < 0.1f) velocityY = 0.0f;
}

inline void handleWallCollision(float& posX, float& posY, 
                                float& velX, float& velY,
                                float radius, const PhysicsConfig& config) {
    // Left/Right walls
    if (posX - radius < 0.0f) {
        posX = radius;
        velX = -velX * config.restitution * config.friction;
        velY *= config.friction;
    }
    else if (posX + radius > config.boxWidth) {
        posX = config.boxWidth - radius;
        velX = -velX * config.restitution * config.friction;
        velY *= config.friction;
    }

    // Top/Bottom walls
    if (posY - radius < 0.0f) {
        posY = radius;
        velY = -velY * config.restitution * config.friction;
        velX *= config.friction;
    }
    else if (posY + radius > config.boxHeight) {
        posY = config.boxHeight - radius;
        velY = -velY * config.restitution * config.friction;
        velX *= config.friction;
    }
}

inline bool checkBallCollision(float x1, float y1, float radius1,
                               float x2, float y2, float radius2,
                               float& distOut, float& nxOut, float& nyOut) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dist = std::sqrt(dx * dx + dy * dy);
    float minDist = radius1 + radius2;

    if (dist < minDist && dist > 0.0f) {
        distOut = dist;
        nxOut = dx / dist;
        nyOut = dy / dist;
        return true;
    }
    return false;
}

inline void resolveBallCollision(float& x1, float& y1, float& vx1, float& vy1, float mass1, float radius1,
                                 float& x2, float& y2, float& vx2, float& vy2, float mass2, float radius2,
                                 float dist, float nx, float ny, float restitution) {
    // Separate balls to prevent overlap
    float minDist = radius1 + radius2;
    float overlap = minDist - dist;
    float totalMass = mass1 + mass2;
    float ratio1 = mass2 / totalMass;
    float ratio2 = mass1 / totalMass;

    x1 -= nx * overlap * ratio1;
    y1 -= ny * overlap * ratio1;
    x2 += nx * overlap * ratio2;
    y2 += ny * overlap * ratio2;

    // Calculate relative velocity
    float dvx = vx2 - vx1;
    float dvy = vy2 - vy1;
    float dvn = dvx * nx + dvy * ny;

    // Don't resolve if velocities are separating
    if (dvn < 0.0f) {
        return;
    }

    // Apply impulse
    float impulse = -(1.0f + restitution) * dvn / totalMass;
    vx1 -= impulse * mass2 * nx;
    vy1 -= impulse * mass2 * ny;
    vx2 += impulse * mass1 * nx;
    vy2 += impulse * mass1 * ny;
}

} // namespace BallsSim
