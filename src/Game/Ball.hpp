// Ball.hpp
// Game entity wrapper around ball physics state.
#pragma once

#include "Core/Types.hpp"
#include "Physics/BallPhysics.hpp"

namespace Sports {

class Ball {
public:
    Ball();

    void update(f32 deltaTime, const FieldBounds& bounds);
    void reset();  // Return to center field

    // Actions
    void kick(const Vec3& direction, f32 power, f32 spinY = 0.0f, f32 spinX = 0.0f);
    void push(const Vec3& direction, f32 force);

    // Getters
    const Vec3& getPosition() const { return m_state.position; }
    const Vec3& getVelocity() const { return m_state.velocity; }
    f32 getRotationAngle() const { return m_state.rotationAngle; }
    bool isInAir() const { return BallPhysics::isInAir(m_state); }
    bool isLow() const { return BallPhysics::isLow(m_state); }

    // Setters
    void setPosition(const Vec3& pos) { m_state.position = pos; }
    void setVelocity(const Vec3& vel) { m_state.velocity = vel; }

    // Direct state access for collision handling
    BallState& state() { return m_state; }
    const BallState& state() const { return m_state; }

    static constexpr f32 RADIUS = BallPhysics::BALL_RADIUS;

private:
    BallState m_state;
};

}
