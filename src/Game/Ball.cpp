// Ball.cpp
// Ball entity implementation.
#include "Ball.hpp"

namespace Sports {

Ball::Ball() {
    reset();
}

void Ball::update(f32 deltaTime, const FieldBounds& bounds) {
    BallPhysics::update(m_state, deltaTime, bounds);
}

void Ball::reset() {
    m_state.position = Vec3(0.0f, 0.5f, 0.0f);  // Center field, slightly elevated
    m_state.velocity = Vec3(0.0f);
    m_state.angularVelocity = Vec3(0.0f);
    m_state.rotationAngle = 0.0f;
}

void Ball::kick(const Vec3& direction, f32 power, f32 spinY, f32 spinX) {
    Vec3 kickDir = glm::normalize(direction);
    m_state.velocity = kickDir * power;
    m_state.angularVelocity = Vec3(spinX, spinY, 0.0f);
}

void Ball::push(const Vec3& direction, f32 force) {
    m_state.velocity += direction * force;
}

}
