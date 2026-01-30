// BallPhysics.hpp
// Soccer ball physics with gravity, drag, Magnus effect, bounce, and friction.
#pragma once

#include "Core/Types.hpp"

namespace Sports {

// Runtime state of the ball
struct BallState {
    Vec3 position{0.0f, 0.5f, 0.0f};
    Vec3 velocity{0.0f, 0.0f, 0.0f};
    Vec3 angularVelocity{0.0f, 0.0f, 0.0f};  // Spin in rad/s
    f32 rotationAngle = 0.0f;                 // Visual rotation
};

// Field dimensions for boundary checks
struct FieldBounds {
    f32 length = 105.0f;    // FIFA standard
    f32 width = 68.0f;
    f32 goalWidth = 7.32f;
    f32 goalHeight = 2.44f;
};

// Static utility class for ball physics calculations
class BallPhysics {
public:
    // Physical constants (SI units)
    static constexpr f32 BALL_RADIUS = 0.22f;        // meters
    static constexpr f32 BALL_MASS = 0.43f;          // kg (FIFA: 410-450g)
    static constexpr f32 GRAVITY = 9.81f;            // m/s²
    static constexpr f32 AIR_DENSITY = 1.2f;         // kg/m³
    static constexpr f32 DRAG_COEFFICIENT = 0.2f;    // Aerodynamic drag
    static constexpr f32 MAGNUS_COEFFICIENT = 0.5f;  // Spin curve strength
    static constexpr f32 BOUNCE_FACTOR = 0.7f;       // Energy retained on bounce
    static constexpr f32 ROLLING_FRICTION = 0.3f;    // Grass friction
    static constexpr f32 SPIN_DECAY = 0.98f;         // Spin reduction per second

    // Main update - applies all physics for one frame
    static void update(BallState& ball, f32 deltaTime, const FieldBounds& bounds);

    // State queries
    static bool isInAir(const BallState& ball);  // Clearly airborne
    static bool isLow(const BallState& ball);    // Safe to kick

private:
    static void applyGravity(BallState& ball, f32 deltaTime);
    static void applyAirDrag(BallState& ball, f32 deltaTime);
    static void applyMagnusEffect(BallState& ball, f32 deltaTime);
    static void applySpinDecay(BallState& ball, f32 deltaTime);
    static void handleGroundCollision(BallState& ball);
    static void applyRollingFriction(BallState& ball, f32 deltaTime);
    static void handleFieldBoundaries(BallState& ball, const FieldBounds& bounds);
};

}
