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

inline void updateSleepState([[maybe_unused]] bool& isAsleep, 
                             [[maybe_unused]] float& sleepTimer, 
                             [[maybe_unused]] float vx, 
                             [[maybe_unused]] float vy, 
                             [[maybe_unused]] float deltaTime, 
                             [[maybe_unused]] const PhysicsConfig& config) {
    // Not used anymore - kept for compatibility
    // Use updateSleepStateWithVariance instead
}

inline void updateSleepStateWithVariance(bool& isAsleep, float& sleepTimer, 
                                         int& sampleCount, float& meanX, float& meanY,
                                         float& m2X, float& m2Y,
                                         float posX, float posY, 
                                         float deltaTime, const PhysicsConfig& config) {
    // Welford's online algorithm for running mean and variance
    // This allows us to detect if a ball is jittering around the same position
    // without storing the full history of positions
    
    sampleCount++;
    
    // Update running mean and M2 (sum of squared differences) for X
    float deltaX = posX - meanX;
    meanX += deltaX / sampleCount;
    float delta2X = posX - meanX;
    m2X += deltaX * delta2X;
    
    // Update running mean and M2 for Y
    float deltaY = posY - meanY;
    meanY += deltaY / sampleCount;
    float delta2Y = posY - meanY;
    m2Y += deltaY * delta2Y;
    
    // Calculate variance (biased toward recent position by design)
    // Note: For small sample counts, this gives population variance
    if (sampleCount >= config.minSamplesForSleep) {
        float varianceX = m2X / sampleCount;
        float varianceY = m2Y / sampleCount;
        float totalVariance = varianceX + varianceY;
        
        // Check if position variance is low enough to sleep
        if (totalVariance < config.sleepVarianceThreshold) {
            sleepTimer += deltaTime;
            if (sleepTimer >= config.sleepTimeThreshold) {
                isAsleep = true;
            }
        } else {
            // Reset sleep timer if variance is too high
            sleepTimer = 0.0f;
            isAsleep = false;
        }
    } else {
        // Not enough samples yet, keep accumulating
        sleepTimer = 0.0f;
        isAsleep = false;
    }
    
    // Limit sample count to prevent overflow and bias toward recent history
    // Reset statistics periodically to adapt to new stable positions
    const int maxSamples = 100;
    if (sampleCount >= maxSamples) {
        // Decay the statistics to bias toward current position
        sampleCount = sampleCount / 2;
        m2X = m2X / 2.0f;
        m2Y = m2Y / 2.0f;
    }
}

inline void wakeUp(bool& isAsleep, float& sleepTimer) {
    isAsleep = false;
    sleepTimer = 0.0f;
}

inline void wakeUpWithVariance(bool& isAsleep, float& sleepTimer,
                                int& sampleCount, float& meanX, float& meanY,
                                float& m2X, float& m2Y) {
    isAsleep = false;
    sleepTimer = 0.0f;
    // Reset variance tracking so it can re-establish at new position
    sampleCount = 0;
    meanX = 0.0f;
    meanY = 0.0f;
    m2X = 0.0f;
    m2Y = 0.0f;
}

inline bool shouldWakeUp(bool isAsleep, float impulseMagnitude, float threshold) {
    return isAsleep && impulseMagnitude > threshold;
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

inline float calculateCollisionImpulse(float vx1, float vy1, float mass1, bool isAsleep1,
                                       float vx2, float vy2, float mass2, bool isAsleep2,
                                       float nx, float ny, float restitution) {
    // Treat sleeping balls as immovable walls (infinite mass)
    const float infiniteMass = 1e10f;
    float effectiveMass1 = isAsleep1 ? infiniteMass : mass1;
    float effectiveMass2 = isAsleep2 ? infiniteMass : mass2;
    
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
    float impulse = -(1.0f + restitution) * dvn / (1.0f / effectiveMass1 + 1.0f / effectiveMass2);
    
    return std::abs(impulse);
}

inline void resolveBallCollision(float& x1, float& y1, float& vx1, float& vy1, float mass1, float radius1, bool isAsleep1,
                                 float& x2, float& y2, float& vx2, float& vy2, float mass2, float radius2, bool isAsleep2,
                                 float dist, float nx, float ny, float restitution) {
    // Separate balls to prevent overlap
    float minDist = radius1 + radius2;
    float overlap = minDist - dist;
    
    // Determine which balls are dynamic (awake)
    bool dynamic1 = !isAsleep1;
    bool dynamic2 = !isAsleep2;
    
    // Position correction based on sleep states
    if (dynamic1 && dynamic2) {
        // Both awake - distribute correction by mass ratio
        float totalMass = mass1 + mass2;
        float ratio1 = mass2 / totalMass;
        float ratio2 = mass1 / totalMass;
        x1 -= nx * overlap * ratio1;
        y1 -= ny * overlap * ratio1;
        x2 += nx * overlap * ratio2;
        y2 += ny * overlap * ratio2;
    } else if (dynamic1) {
        // Only ball 1 is awake - it moves entirely
        x1 -= nx * overlap;
        y1 -= ny * overlap;
    } else if (dynamic2) {
        // Only ball 2 is awake - it moves entirely
        x2 += nx * overlap;
        y2 += ny * overlap;
    }
    // If both asleep, no position correction

    // Calculate relative velocity along collision normal
    float dvx = vx2 - vx1;
    float dvy = vy2 - vy1;
    float dvn = dvx * nx + dvy * ny;

    // Don't resolve if velocities are separating
    if (dvn >= 0.0f) {
        return;
    }

    // Calculate impulse based on sleep states
    float impulse;
    if (dynamic1 && dynamic2) {
        // Both awake - standard two-body collision
        impulse = -(1.0f + restitution) * dvn / (1.0f / mass1 + 1.0f / mass2);
        
        // Apply impulse to both objects
        vx1 -= (impulse / mass1) * nx;
        vy1 -= (impulse / mass1) * ny;
        vx2 += (impulse / mass2) * nx;
        vy2 += (impulse / mass2) * ny;
    } else if (dynamic1) {
        // Ball 1 awake, ball 2 is wall - simpler calculation
        impulse = (1.0f + restitution) * dvn;
        vx1 -= impulse * nx;
        vy1 -= impulse * ny;
    } else if (dynamic2) {
        // Ball 2 awake, ball 1 is wall
        impulse = -(1.0f + restitution) * dvn;
        vx2 += impulse * nx;
        vy2 += impulse * ny;
    }
    
    // Clamp velocities to prevent energy gain from numerical errors
    const float maxVelocity = 2000.0f;
    if (dynamic1) {
        float v1Mag = std::sqrt(vx1 * vx1 + vy1 * vy1);
        if (v1Mag > maxVelocity) {
            vx1 = (vx1 / v1Mag) * maxVelocity;
            vy1 = (vy1 / v1Mag) * maxVelocity;
        }
    }
    if (dynamic2) {
        float v2Mag = std::sqrt(vx2 * vx2 + vy2 * vy2);
        if (v2Mag > maxVelocity) {
            vx2 = (vx2 / v2Mag) * maxVelocity;
            vy2 = (vy2 / v2Mag) * maxVelocity;
        }
    }
}

} // namespace BallsSim
