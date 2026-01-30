// Player.hpp
// Human-controlled player with movement, dribbling, and kicking.
#pragma once

#include "Core/Types.hpp"
#include "Ball.hpp"

namespace Sports {

class Player {
public:
    // Movement tuning (m/s)
    static constexpr f32 MAX_SPEED = 8.0f;
    static constexpr f32 SPRINT_SPEED = 12.0f;
    static constexpr f32 ACCELERATION = 40.0f;
    static constexpr f32 DECELERATION = 30.0f;
    static constexpr f32 ROTATION_SPEED = 6.0f;  // Radians per second
    static constexpr f32 RADIUS = 0.3f;          // Collision radius
    static constexpr f32 KICK_RANGE = 1.5f;
    static constexpr f32 DRIBBLE_RANGE = 1.2f;

    Player();

    void update(f32 deltaTime, const Vec3& fieldBoundsMin, const Vec3& fieldBoundsMax);

    // Input
    void setMovementInput(const Vec3& direction, bool sprinting);
    void setTargetRotation(f32 rotation);

    // Ball interaction
    bool tryKick(Ball& ball, bool sprinting, f32 spinY);
    void handleBallCollision(Ball& ball, f32 deltaTime);

    // Getters
    const Vec3& getPosition() const { return m_position; }
    const Vec3& getVelocity() const { return m_velocity; }
    f32 getRotation() const { return m_rotation; }
    f32 getSpeed() const { return glm::length(m_velocity); }
    f32 getAnimationTime() const { return m_animationTime; }
    bool isKicking() const { return m_isKicking; }
    f32 getKickTimer() const { return m_kickAnimationTimer; }

    void setPosition(const Vec3& pos) { m_position = pos; }

private:
    void updateMovement(f32 deltaTime);
    void updateRotation(f32 deltaTime);
    void updateAnimation(f32 deltaTime);
    void clampToBounds(const Vec3& boundsMin, const Vec3& boundsMax);
    void dribble(Ball& ball, f32 deltaTime);

    // Transform
    Vec3 m_position{0.0f, 0.0f, 5.0f};
    Vec3 m_velocity{0.0f};
    f32 m_rotation = 0.0f;
    f32 m_targetRotation = 0.0f;

    // Input state
    Vec3 m_inputDirection{0.0f};
    bool m_isSprinting = false;

    // Animation
    f32 m_animationTime = 0.0f;
    f32 m_kickAnimationTimer = 0.0f;
    bool m_isKicking = false;
};

}
