// Player.cpp
// Human player movement, dribbling, and kick mechanics.
#include "Player.hpp"
#include <cmath>
#include <algorithm>
#include <cstdlib>

namespace Sports {

Player::Player() = default;

void Player::update(f32 deltaTime, const Vec3& boundsMin, const Vec3& boundsMax) {
    updateMovement(deltaTime);
    updateRotation(deltaTime);
    updateAnimation(deltaTime);
    clampToBounds(boundsMin, boundsMax);
}

void Player::setMovementInput(const Vec3& direction, bool sprinting) {
    m_inputDirection = direction;
    m_isSprinting = sprinting;
}

void Player::setTargetRotation(f32 rotation) {
    m_targetRotation = rotation;
}

bool Player::tryKick(Ball& ball, bool sprinting, f32 spinY) {
    f32 distToBall = glm::length(ball.getPosition() - m_position);
    if (distToBall >= KICK_RANGE) {
        return false;
    }

    // Kick in facing direction with slight upward angle
    Vec3 kickDir(-std::sin(m_rotation), 0.3f, -std::cos(m_rotation));
    kickDir = glm::normalize(kickDir);

    f32 kickPower = sprinting ? 22.0f : 15.0f;
    f32 spinX = sprinting ? -5.0f : 0.0f;  // Topspin on power shots

    ball.kick(kickDir, kickPower, spinY, spinX);

    m_isKicking = true;
    m_kickAnimationTimer = 0.3f;

    return true;
}

void Player::handleBallCollision(Ball& ball, f32 deltaTime) {
    Vec3 toBall = ball.getPosition() - m_position;
    toBall.y = 0;
    f32 distToBall = glm::length(toBall);

    f32 minDist = RADIUS + Ball::RADIUS;
    bool ballOnGround = ball.getPosition().y <= Ball::RADIUS + 0.1f;

    // Dribbling: guide ball while moving
    if (distToBall < DRIBBLE_RANGE && getSpeed() > 0.5f && ballOnGround) {
        dribble(ball, deltaTime);
    }

    // Hard collision: prevent walking through ball
    if (distToBall < minDist && distToBall > 0.01f) {
        Vec3 pushDir = glm::normalize(toBall);
        ball.state().position.x = m_position.x + pushDir.x * minDist;
        ball.state().position.z = m_position.z + pushDir.z * minDist;

        // Transfer some momentum
        if (getSpeed() > 0.5f) {
            ball.state().velocity.x += pushDir.x * getSpeed() * 0.3f;
            ball.state().velocity.z += pushDir.z * getSpeed() * 0.3f;
        }
    }
}

void Player::updateMovement(f32 deltaTime) {
    f32 targetSpeed = m_isSprinting ? SPRINT_SPEED : MAX_SPEED;

    if (glm::length(m_inputDirection) > 0.01f) {
        Vec3 inputDir = glm::normalize(m_inputDirection);
        Vec3 targetVelocity = inputDir * targetSpeed;

        // Accelerate toward target velocity
        Vec3 velocityDiff = targetVelocity - m_velocity;
        f32 accelAmount = ACCELERATION * deltaTime;

        if (glm::length(velocityDiff) < accelAmount) {
            m_velocity = targetVelocity;
        } else {
            m_velocity += glm::normalize(velocityDiff) * accelAmount;
        }
    } else {
        // Decelerate when no input
        f32 speed = glm::length(m_velocity);
        if (speed > 0.01f) {
            f32 decelAmount = DECELERATION * deltaTime;
            if (speed < decelAmount) {
                m_velocity = Vec3(0.0f);
            } else {
                m_velocity -= glm::normalize(m_velocity) * decelAmount;
            }
        }
    }

    m_position += m_velocity * deltaTime;
}

void Player::updateRotation(f32 deltaTime) {
    f32 rotationDiff = m_targetRotation - m_rotation;

    // Wrap angle to [-PI, PI]
    while (rotationDiff > 3.14159f) rotationDiff -= 6.28318f;
    while (rotationDiff < -3.14159f) rotationDiff += 6.28318f;

    // Smooth rotation using exponential decay (frame-rate independent)
    f32 t = 1.0f - std::exp(-ROTATION_SPEED * deltaTime);
    m_rotation += rotationDiff * t;

    // Keep rotation in valid range
    while (m_rotation > 3.14159f) m_rotation -= 6.28318f;
    while (m_rotation < -3.14159f) m_rotation += 6.28318f;
}

void Player::updateAnimation(f32 deltaTime) {
    f32 moveSpeed = glm::length(m_velocity);
    if (moveSpeed > 0.5f) {
        m_animationTime += deltaTime * moveSpeed * 0.8f;
    }

    if (m_kickAnimationTimer > 0) {
        m_kickAnimationTimer -= deltaTime;
        if (m_kickAnimationTimer <= 0) {
            m_isKicking = false;
        }
    }
}

void Player::clampToBounds(const Vec3& boundsMin, const Vec3& boundsMax) {
    m_position.x = std::clamp(m_position.x, boundsMin.x, boundsMax.x);
    m_position.z = std::clamp(m_position.z, boundsMin.z, boundsMax.z);
}

void Player::dribble(Ball& ball, f32 deltaTime) {
    Vec3 toBall = ball.getPosition() - m_position;
    toBall.y = 0;

    // Calculate ideal ball position: slightly in front of player
    Vec3 playerForward(-std::sin(m_rotation), 0.0f, -std::cos(m_rotation));
    Vec3 idealBallPos = m_position + playerForward * 0.8f;
    idealBallPos.y = Ball::RADIUS;

    // Gently push ball toward ideal position
    f32 dribbleControl = 0.15f;
    Vec3 toIdeal = idealBallPos - ball.getPosition();
    toIdeal.y = 0;

    // Only dribble if ball is roughly in front
    f32 dotProduct = glm::dot(glm::normalize(toBall), playerForward);
    if (dotProduct > -0.3f) {
        ball.state().velocity.x += toIdeal.x * dribbleControl * getSpeed();
        ball.state().velocity.z += toIdeal.z * dribbleControl * getSpeed();

        // Add small random touches for realism
        static f32 touchTimer = 0.0f;
        touchTimer += deltaTime;
        if (touchTimer > 0.15f) {
            touchTimer = 0.0f;
            f32 touchStrength = 0.5f + (std::rand() % 100) / 200.0f;
            ball.state().velocity += playerForward * getSpeed() * touchStrength * 0.3f;
        }

        // Limit ball speed to prevent outrunning dribble
        f32 ballSpeed = glm::length(Vec3(ball.getVelocity().x, 0, ball.getVelocity().z));
        if (ballSpeed > getSpeed() * 1.5f) {
            f32 factor = getSpeed() * 1.5f / ballSpeed;
            ball.state().velocity.x *= factor;
            ball.state().velocity.z *= factor;
        }
    }
}

}
