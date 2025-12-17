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
    
    // Stop very slow movement - lower threshold for better stability
    if (std::abs(velocityX) < 0.5f) velocityX = 0.0f;
    if (std::abs(velocityY) < 0.5f) velocityY = 0.0f;
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

inline float calculateCollisionImpulse(float vx1, float vy1, float mass1,
                                       float vx2, float vy2, float mass2,
                                       float nx, float ny, float restitution) {
    // Calculate relative velocity along collision normal
    float dvx = vx2 - vx1;
    float dvy = vy2 - vy1;
    float dvn = dvx * nx + dvy * ny;

    // Don't calculate impulse if velocities are separating
    if (dvn >= 0.0f) {
        return 0.0f;
    }

    // Calculate impulse magnitude using correct physics formula
    // J = -(1 + e) * v_rel · n / (1/m1 + 1/m2)
    float impulse = -(1.0f + restitution) * dvn / (1.0f / mass1 + 1.0f / mass2);
    
    return std::abs(impulse);
}

inline void resolveBallCollision(float& x1, float& y1, float& vx1, float& vy1, float mass1, float radius1,
                                 float& x2, float& y2, float& vx2, float& vy2, float mass2, float radius2,
                                 float dist, float nx, float ny, float restitution) {
    // Separate balls to prevent overlap
    float minDist = radius1 + radius2;
    float overlap = minDist - dist;
    
    // Position correction - distribute by mass ratio
    float totalMass = mass1 + mass2;
    float ratio1 = mass2 / totalMass;
    float ratio2 = mass1 / totalMass;
    x1 -= nx * overlap * ratio1;
    y1 -= ny * overlap * ratio1;
    x2 += nx * overlap * ratio2;
    y2 += ny * overlap * ratio2;

    // Calculate relative velocity along collision normal
    float dvx = vx2 - vx1;
    float dvy = vy2 - vy1;
    float dvn = dvx * nx + dvy * ny;

    // Don't resolve if velocities are separating
    if (dvn >= 0.0f) {
        return;
    }

    // Calculate and apply impulse
    float impulse = -(1.0f + restitution) * dvn / (1.0f / mass1 + 1.0f / mass2);
    
    vx1 -= (impulse / mass1) * nx;
    vy1 -= (impulse / mass1) * ny;
    vx2 += (impulse / mass2) * nx;
    vy2 += (impulse / mass2) * ny;
    
    // Clamp velocities to prevent energy gain from numerical errors
    const float maxVelocity = 2000.0f;
    float v1Mag = std::sqrt(vx1 * vx1 + vy1 * vy1);
    if (v1Mag > maxVelocity) {
        vx1 = (vx1 / v1Mag) * maxVelocity;
        vy1 = (vy1 / v1Mag) * maxVelocity;
    }
    float v2Mag = std::sqrt(vx2 * vx2 + vy2 * vy2);
    if (v2Mag > maxVelocity) {
        vx2 = (vx2 / v2Mag) * maxVelocity;
        vy2 = (vy2 / v2Mag) * maxVelocity;
    }
}

} // namespace BallsSim
