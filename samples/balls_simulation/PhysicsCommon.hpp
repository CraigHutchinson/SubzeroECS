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

// ============================================================================
// Swept Circle Collision Detection (prevents tunneling at high speeds)
// ============================================================================
// Checks if a moving circle (ball1) collides with a moving circle (ball2) during the timestep
// Returns true if collision occurs, and outputs the collision time (0-1), collision point, and normal
inline bool checkSweptCircleCollision(float x1, float y1, float vx1, float vy1, float radius1,
                                      float x2, float y2, float vx2, float vy2, float radius2,
                                      float deltaTime,
                                      float& tCollisionOut, float& distOut, 
                                      float& nxOut, float& nyOut) {
    // Relative position and relative velocity (scaled by deltaTime to get displacement)
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dvx = (vx1 - vx2) * deltaTime;  // Relative velocity scaled by time
    float dvy = (vy1 - vy2) * deltaTime;
    
    float sumRadius = radius1 + radius2;
    
    // Quadratic equation: |dx + dvx*t|^2 = (sumRadius)^2
    // Where t is normalized time [0, 1] over the timestep
    // a*t^2 + b*t + c = 0
    float a = dvx * dvx + dvy * dvy;
    float b = 2.0f * (dx * dvx + dy * dvy);
    float c = dx * dx + dy * dy - sumRadius * sumRadius;
    
    // Check if already overlapping
    if (c < 0.0f) {
        tCollisionOut = 0.0f;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist > 0.0001f) {
            nxOut = dx / dist;
            nyOut = dy / dist;
            distOut = dist;
            return true;
        }
    }
    
    // Check if there's a collision solution
    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f) {
        return false;
    }
    
    float sqrtDisc = std::sqrt(discriminant);
    float t1 = (-b - sqrtDisc) / (2.0f * a);
    float t2 = (-b + sqrtDisc) / (2.0f * a);
    
    // We want the earliest collision time in range [0, 1]
    float tCollision = -1.0f;
    if (t1 >= 0.0f && t1 <= 1.0f) {
        tCollision = t1;
    } else if (t2 >= 0.0f && t2 <= 1.0f) {
        tCollision = t2;
    } else {
        return false; // No collision in this timestep
    }
    
    // Calculate collision position (in ball1's frame)
    float colX = x1 + vx1 * deltaTime * tCollision;
    float colY = y1 + vy1 * deltaTime * tCollision;
    
    // Calculate collision position (in ball2's frame) for verification
    float col2X = x2 + vx2 * deltaTime * tCollision;
    float col2Y = y2 + vy2 * deltaTime * tCollision;
    
    // Calculate normal (from ball1 to ball2)
    float nx = col2X - colX;
    float ny = col2Y - colY;
    float dist = std::sqrt(nx * nx + ny * ny);
    
    if (dist > 0.0001f) {
        nxOut = nx / dist;
        nyOut = ny / dist;
        distOut = dist;
        tCollisionOut = tCollision;
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
