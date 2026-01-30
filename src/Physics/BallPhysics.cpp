// BallPhysics.cpp
// Implementation of realistic soccer ball physics.
#include "BallPhysics.hpp"
#include <cmath>
#include <algorithm>

namespace Sports {

void BallPhysics::update(BallState& ball, f32 deltaTime, const FieldBounds& bounds) {
    bool inAir = isInAir(ball);
    f32 ballSpeed = glm::length(ball.velocity);

    // Apply forces
    applyGravity(ball, deltaTime);

    if (inAir && ballSpeed > 0.1f) {
        applyAirDrag(ball, deltaTime);
    }

    // Magnus effect: F = S * (omega x v) causes spin curve
    if (inAir && glm::length(ball.angularVelocity) > 0.1f && ballSpeed > 1.0f) {
        applyMagnusEffect(ball, deltaTime);
    }

    applySpinDecay(ball, deltaTime);

    // Integrate position
    ball.position += ball.velocity * deltaTime;

    // Update visual rotation based on movement
    if (ballSpeed > 0.1f) {
        ball.rotationAngle += ballSpeed * deltaTime * 3.0f;
    }

    // Handle collisions
    handleGroundCollision(ball);
    applyRollingFriction(ball, deltaTime);
    handleFieldBoundaries(ball, bounds);
}

bool BallPhysics::isInAir(const BallState& ball) {
    const f32 airThreshold = BALL_RADIUS + 0.3f;
    return ball.position.y > airThreshold;
}

bool BallPhysics::isLow(const BallState& ball) {
    // Must be close to ground AND not moving upward (prevents kicking mid-bounce)
    return ball.position.y < BALL_RADIUS + 0.15f && ball.velocity.y < 0.5f;
}

void BallPhysics::applyGravity(BallState& ball, f32 deltaTime) {
    ball.velocity.y -= GRAVITY * deltaTime;
}

void BallPhysics::applyAirDrag(BallState& ball, f32 deltaTime) {
    f32 ballSpeed = glm::length(ball.velocity);
    if (ballSpeed < 0.01f) return;

    // Drag equation: F = 0.5 * rho * Cd * A * v²
    Vec3 velDir = ball.velocity / ballSpeed;
    f32 ballArea = 3.14159f * BALL_RADIUS * BALL_RADIUS;
    f32 dragForce = 0.5f * AIR_DENSITY * DRAG_COEFFICIENT * ballArea * ballSpeed * ballSpeed;
    f32 dragAccel = dragForce / BALL_MASS;

    ball.velocity -= velDir * dragAccel * deltaTime;
}

void BallPhysics::applyMagnusEffect(BallState& ball, f32 deltaTime) {
    // Magnus force perpendicular to both spin axis and velocity
    Vec3 magnusForce = MAGNUS_COEFFICIENT * glm::cross(ball.angularVelocity, ball.velocity);

    // Clamp to prevent unrealistic curves
    f32 maxMagnusAccel = 15.0f;
    f32 magnusAccel = glm::length(magnusForce) / BALL_MASS;
    if (magnusAccel > maxMagnusAccel) {
        magnusForce *= maxMagnusAccel / magnusAccel;
    }

    ball.velocity += (magnusForce / BALL_MASS) * deltaTime;
}

void BallPhysics::applySpinDecay(BallState& ball, f32 deltaTime) {
    // Spin decays faster on ground due to friction
    if (isInAir(ball)) {
        ball.angularVelocity *= std::pow(SPIN_DECAY, deltaTime);
    } else {
        ball.angularVelocity *= std::pow(0.9f, deltaTime);
    }
}

void BallPhysics::handleGroundCollision(BallState& ball) {
    const f32 groundY = BALL_RADIUS;

    if (ball.position.y < groundY) {
        ball.position.y = groundY;

        // Bounce if moving downward fast enough
        if (ball.velocity.y < -0.5f) {
            ball.velocity.y = -ball.velocity.y * BOUNCE_FACTOR;
            ball.velocity.x *= 0.9f;  // Friction on bounce
            ball.velocity.z *= 0.9f;
            ball.angularVelocity *= 0.7f;
        } else {
            ball.velocity.y = 0.0f;
        }
    }
}

void BallPhysics::applyRollingFriction(BallState& ball, f32 deltaTime) {
    const f32 groundY = BALL_RADIUS;

    if (ball.position.y <= groundY + 0.05f) {
        f32 groundSpeed = glm::length(Vec3(ball.velocity.x, 0.0f, ball.velocity.z));
        if (groundSpeed > 0.01f) {
            f32 frictionDecel = ROLLING_FRICTION * GRAVITY * deltaTime;
            f32 newSpeed = std::max(0.0f, groundSpeed - frictionDecel);
            f32 frictionFactor = newSpeed / groundSpeed;
            ball.velocity.x *= frictionFactor;
            ball.velocity.z *= frictionFactor;
        } else {
            ball.velocity.x = 0.0f;
            ball.velocity.z = 0.0f;
        }
    }
}

void BallPhysics::handleFieldBoundaries(BallState& ball, const FieldBounds& bounds) {
    f32 halfLength = bounds.length / 2.0f;
    f32 halfWidth = bounds.width / 2.0f;
    f32 goalHalfW = bounds.goalWidth / 2.0f;

    // X boundaries (end lines) - allow through if in goal area
    if (std::abs(ball.position.x) > halfLength) {
        bool inGoalArea = std::abs(ball.position.z) < goalHalfW &&
                          ball.position.y < bounds.goalHeight;
        if (!inGoalArea) {
            ball.position.x = std::copysign(halfLength, ball.position.x);
            ball.velocity.x = -ball.velocity.x * 0.5f;
            ball.angularVelocity *= 0.5f;
        }
    }

    // Z boundaries (side lines)
    if (std::abs(ball.position.z) > halfWidth + 0.5f) {
        ball.position.z = std::copysign(halfWidth + 0.5f, ball.position.z);
        ball.velocity.z = -ball.velocity.z * 0.5f;
        ball.angularVelocity *= 0.5f;
    }
}

}
