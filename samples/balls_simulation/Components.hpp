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

// ============================================================================
// Physics Constants
// ============================================================================

struct PhysicsConfig {
    float gravity = 980.0f;        // pixels/s^2 (roughly Earth gravity scaled)
    float damping = 0.98f;         // velocity damping (0-1)
    float restitution = 0.85f;     // bounciness (0-1)
    float friction = 0.99f;        // friction coefficient (0-1)
    float boxWidth = 1600.0f;      // simulation box width
    float boxHeight = 900.0f;      // simulation box height
    float minRadius = 5.0f;
    float maxRadius = 30.0f;
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
