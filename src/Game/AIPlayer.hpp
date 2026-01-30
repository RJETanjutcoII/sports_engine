// AIPlayer.hpp
// AI-controlled players with state machine behavior and team coordination.
#pragma once

#include "Core/Types.hpp"
#include "Ball.hpp"
#include <vector>

namespace Sports {

class AIPlayer {
public:
    // Behavioral states for AI decision-making
    enum class State { Idle, ChaseBall, ReturnToPosition, Defend };

    // Movement tuning (slightly slower than human player for balance)
    static constexpr f32 MAX_SPEED = 7.0f;
    static constexpr f32 ACCELERATION = 25.0f;
    static constexpr f32 KICK_POWER = 15.0f;
    static constexpr f32 KICK_RANGE = 1.0f;
    static constexpr f32 ROTATION_SPEED = 8.0f;
    static constexpr f32 KICK_COOLDOWN = 1.5f;   // Prevents rapid-fire kicks
    static constexpr f32 RADIUS = 0.3f;

    AIPlayer();

    void setHomePosition(const Vec3& pos);
    void setTeam(i32 team) { m_team = team; }
    void setIsClosestChaser(bool isClosest) { m_isClosestChaser = isClosest; }

    void update(f32 deltaTime, Ball& ball, const Vec3& playerPos, f32 fieldLength, f32 fieldWidth, f32 goalWidth);

    // Collision handlers for ball and other entities
    void handleBallCollision(Ball& ball);
    void handlePlayerCollision(const Vec3& playerPos);
    void handleAICollision(AIPlayer& other);

    // Getters
    const Vec3& getPosition() const { return m_position; }
    const Vec3& getVelocity() const { return m_velocity; }
    const Vec3& getHomePosition() const { return m_homePosition; }
    f32 getRotation() const { return m_rotation; }
    f32 getAnimTime() const { return m_animTime; }
    i32 getTeam() const { return m_team; }
    State getState() const { return m_state; }

    f32 distanceToBall(const Vec3& ballPos) const;

private:
    void decideAction(const Vec3& ballPos, const Vec3& ballVel, f32 fieldLength);
    void chaseBall(const Vec3& ballPos, const Vec3& ballVel);
    void returnToPosition(const Vec3& ballPos, f32 fieldLength, f32 goalWidth);
    void moveToward(const Vec3& target, f32 targetSpeed, f32 deltaTime);
    void tryKick(Ball& ball, f32 fieldLength);

    Vec3 m_position{0.0f};
    Vec3 m_velocity{0.0f};
    Vec3 m_homePosition{0.0f};       // Formation position to return to
    f32 m_rotation = 0.0f;
    f32 m_targetRotation = 0.0f;

    State m_state = State::Idle;
    i32 m_team = 0;                  // 0 = red, 1 = blue
    f32 m_kickCooldown = 0.0f;
    f32 m_animTime = 0.0f;

    bool m_isClosestChaser = false;  // Only closest player per team chases
    bool m_isGoalkeeper = false;     // Determined by home position
    bool m_isDefender = false;

    Vec3 m_targetPos{0.0f};
    f32 m_currentTargetSpeed = MAX_SPEED;
};

// Manages all AI players and coordinates team behavior
class AIManager {
public:
    void createTeams(f32 fieldLength);
    void update(f32 deltaTime, Ball& ball, const Vec3& playerPos, f32 fieldLength, f32 fieldWidth, f32 goalWidth);

    std::vector<AIPlayer>& getPlayers() { return m_players; }
    const std::vector<AIPlayer>& getPlayers() const { return m_players; }

private:
    void findClosestChasers(const Vec3& ballPos);
    void handleCollisions(Ball& ball, const Vec3& playerPos);

    std::vector<AIPlayer> m_players;
};

}
