#pragma once

#include <cmath>

namespace BallsSim {

// ============================================================================
// Common Components
// ============================================================================

struct Position {
    float x, y;
};

struct Velocity {
    float dx, dy;
};

struct Radius {
    float value;
};

struct Color {
    uint8_t r, g, b, a;
};

struct Mass {
    float value;
};

struct SleepState {
    bool isAsleep = false;
    float sleepTimer = 0.0f;  // Time the ball has been nearly stationary
    
    // Running statistics for variance-based sleep detection (Welford's algorithm)
    int sampleCount = 0;       // Number of position samples taken
    float meanX = 0.0f;        // Running mean of X position
    float meanY = 0.0f;        // Running mean of Y position
    float m2X = 0.0f;          // Running sum of squared differences for X (for variance)
    float m2Y = 0.0f;          // Running sum of squared differences for Y (for variance)
};

// ============================================================================
// Physics Constants
// ============================================================================

struct PhysicsConfig {
    float gravity = 980.0f;        // pixels/s^2 (roughly Earth gravity scaled)
    float damping = 0.95f;         // velocity damping (0-1) - reduced for faster energy dissipation
    float restitution = 0.3f;      // bounciness (0-1) - reduced to prevent bouncing in piles
    float friction = 0.98f;        // friction coefficient (0-1) - slightly reduced
    float boxWidth = 1600.0f;      // simulation box width
    float boxHeight = 900.0f;      // simulation box height
    float minRadius = 5.0f;
    float maxRadius = 30.0f;
    
    // Sleep parameters
    float sleepVarianceThreshold = 4.0f;   // Max position variance to consider sleeping (pixels^2)
    float sleepTimeThreshold = 0.3f;       // Time to stay still before sleeping (seconds)
    int minSamplesForSleep = 10;           // Minimum samples needed before checking variance
    
    // Constraint solving parameters
    int collisionIterations = 3;           // Number of iterations for collision resolution
    
    // Calculate wake-up impulse threshold
    // For a typical collision, impulse ≈ mass * delta_velocity
    // We want sufficient impulse to move the ball from its stable position
    float getWakeUpImpulseThreshold(float typicalMass = 10.0f) const {
        // Require enough impulse to potentially exceed the position variance threshold
        // Approximate: sqrt(sleepVarianceThreshold) gives std dev in pixels
        // Convert to velocity threshold and then impulse
        float approxVelocityThreshold = std::sqrt(sleepVarianceThreshold) * 2.0f;
        return typicalMass * approxVelocityThreshold;
    }
};

// ============================================================================
// Utility Functions
// ============================================================================

inline float distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

inline void normalize(float& x, float& y) {
    float len = std::sqrt(x * x + y * y);
    if (len > 0.0f) {
        x /= len;
        y /= len;
    }
}

} // namespace BallsSim
