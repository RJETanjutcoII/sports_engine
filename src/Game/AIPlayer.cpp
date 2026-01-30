// AIPlayer.cpp
// AI player decision-making, movement, and team management.
#include "AIPlayer.hpp"
#include <cmath>
#include <algorithm>
#include <cstdlib>

namespace Sports {

AIPlayer::AIPlayer() = default;

void AIPlayer::setHomePosition(const Vec3& pos) {
    m_homePosition = pos;
    m_position = pos;
    // Infer role from field position (X > 40 = near goal = goalkeeper)
    m_isGoalkeeper = std::abs(pos.x) > 40.0f;
    m_isDefender = std::abs(pos.x) > 30.0f && !m_isGoalkeeper;
}

void AIPlayer::update(f32 deltaTime, Ball& ball, const Vec3& playerPos,
                      f32 fieldLength, f32 fieldWidth, f32 goalWidth) {
    if (m_kickCooldown > 0) {
        m_kickCooldown -= deltaTime;
    }

    decideAction(ball.getPosition(), ball.getVelocity(), fieldLength);
    moveToward(m_targetPos, m_currentTargetSpeed, deltaTime);

    // Only attempt kick when ball is grounded (isLow prevents mid-air kicks)
    f32 dist = distanceToBall(ball.getPosition());
    if (dist < KICK_RANGE && m_kickCooldown <= 0 && ball.isLow()) {
        tryKick(ball, fieldLength);
    }

    // Animate legs based on movement speed
    f32 speed = glm::length(m_velocity);
    if (speed > 0.5f) {
        m_animTime += deltaTime * speed * 0.8f;
    }

    // Keep within field boundaries
    m_position.x = std::clamp(m_position.x, -fieldLength / 2.0f + 1.0f, fieldLength / 2.0f - 1.0f);
    m_position.z = std::clamp(m_position.z, -fieldWidth / 2.0f + 1.0f, fieldWidth / 2.0f - 1.0f);

    // Smooth rotation using exponential decay (frame-rate independent)
    f32 rotDiff = m_targetRotation - m_rotation;
    while (rotDiff > 3.14159f) rotDiff -= 6.28318f;
    while (rotDiff < -3.14159f) rotDiff += 6.28318f;
    f32 rotT = 1.0f - std::exp(-ROTATION_SPEED * deltaTime);
    m_rotation += rotDiff * rotT;
}

f32 AIPlayer::distanceToBall(const Vec3& ballPos) const {
    Vec3 toBall = ballPos - m_position;
    toBall.y = 0;  // Ignore vertical distance for ground-based checks
    return glm::length(toBall);
}

void AIPlayer::decideAction(const Vec3& ballPos, const Vec3& ballVel, f32 fieldLength) {
    f32 dist = distanceToBall(ballPos);
    f32 ballX = ballPos.x;

    bool shouldChase = false;

    if (m_isGoalkeeper) {
        // Goalkeeper only engages when ball is near their goal
        f32 goalX = (m_team == 0) ? -fieldLength / 2.0f : fieldLength / 2.0f;
        if (std::abs(ballX - goalX) < 20.0f && dist < 15.0f) {
            shouldChase = true;
        }
    } else if (m_isClosestChaser) {
        // Only the closest outfield player per team actively chases
        // Prevents crowding by limiting pursuit to offensive half
        if (m_team == 0) {
            if (ballX < 40.0f) shouldChase = true;
        } else {
            if (ballX > -40.0f) shouldChase = true;
        }
    }

    if (shouldChase && dist < 35.0f) {
        m_state = State::ChaseBall;
        chaseBall(ballPos, ballVel);
    } else {
        m_state = State::ReturnToPosition;
        returnToPosition(ballPos, fieldLength, 7.32f);
    }
}

void AIPlayer::chaseBall(const Vec3& ballPos, const Vec3& ballVel) {
    // Predict where ball will be in 0.2s for interception
    Vec3 predictedBall = ballPos + ballVel * 0.2f;
    predictedBall.y = 0;
    m_targetPos = predictedBall;
    m_currentTargetSpeed = MAX_SPEED;
}

void AIPlayer::returnToPosition(const Vec3& ballPos, f32 fieldLength, f32 goalWidth) {
    // Shift formation based on ball position (compact play)
    Vec3 shiftedHome = m_homePosition;
    f32 shiftAmount = ballPos.x * 0.2f;

    if (m_isGoalkeeper) {
        // Goalkeeper tracks ball laterally within goal area
        shiftedHome.z = ballPos.z * 0.5f;
        shiftedHome.z = std::clamp(shiftedHome.z, -goalWidth / 2.0f + 1.0f, goalWidth / 2.0f - 1.0f);
    } else if (m_isDefender) {
        // Defenders shift less aggressively
        shiftedHome.x += shiftAmount * 0.3f;
    } else {
        // Midfielders and forwards shift more with play
        shiftedHome.x += shiftAmount * 0.5f;
    }

    m_targetPos = shiftedHome;
    m_currentTargetSpeed = MAX_SPEED * 0.5f;  // Jog back to position
}

void AIPlayer::moveToward(const Vec3& target, f32 targetSpeed, f32 deltaTime) {
    Vec3 toTarget = target - m_position;
    toTarget.y = 0;
    f32 distToTarget = glm::length(toTarget);

    if (distToTarget > 0.5f) {
        Vec3 moveDir = glm::normalize(toTarget);

        // Slow down when approaching target to prevent oscillation
        Vec3 targetVel = moveDir * std::min(distToTarget * 2.0f, targetSpeed);
        Vec3 velDiff = targetVel - m_velocity;
        f32 accelAmount = ACCELERATION * deltaTime;

        if (glm::length(velDiff) < accelAmount) {
            m_velocity = targetVel;
        } else {
            m_velocity += glm::normalize(velDiff) * accelAmount;
        }

        // Face movement direction
        m_targetRotation = std::atan2(-moveDir.x, -moveDir.z);
    } else {
        // Arrived at target, decelerate
        f32 speed = glm::length(m_velocity);
        if (speed > 0.1f) {
            m_velocity *= 0.9f;
        } else {
            m_velocity = Vec3(0.0f);
        }
    }

    m_position += m_velocity * deltaTime;
}

void AIPlayer::tryKick(Ball& ball, f32 fieldLength) {
    // Calculate direction toward opponent's goal
    Vec3 goalDir;
    if (m_team == 0) {
        goalDir = Vec3(fieldLength / 2.0f, 0, 0) - m_position;
    } else {
        goalDir = Vec3(-fieldLength / 2.0f, 0, 0) - m_position;
    }
    goalDir.y = 0;
    goalDir = glm::normalize(goalDir);

    // Add slight randomness to prevent predictable shots
    goalDir.z += ((std::rand() % 100) - 50) / 100.0f * 0.3f;
    goalDir = glm::normalize(goalDir);

    ball.state().velocity = goalDir * KICK_POWER;
    ball.state().velocity.y = 1.0f;  // Slight lift
    m_kickCooldown = KICK_COOLDOWN;
}

void AIPlayer::handleBallCollision(Ball& ball) {
    // Push ball away when overlapping (prevents walking through ball)
    f32 dist = distanceToBall(ball.getPosition());
    if (dist < 0.5f && dist > 0.01f) {
        Vec3 toBall = ball.getPosition() - m_position;
        toBall.y = 0;
        Vec3 pushDir = glm::normalize(toBall);
        ball.state().position.x = m_position.x + pushDir.x * 0.5f;
        ball.state().position.z = m_position.z + pushDir.z * 0.5f;
    }
}

void AIPlayer::handlePlayerCollision(const Vec3& playerPos) {
    // AI avoids human player by pushing itself away
    Vec3 toPlayer = playerPos - m_position;
    toPlayer.y = 0;
    f32 dist = glm::length(toPlayer);
    if (dist < 0.8f && dist > 0.01f) {
        Vec3 pushDir = glm::normalize(toPlayer);
        m_position -= pushDir * (0.8f - dist);
    }
}

void AIPlayer::handleAICollision(AIPlayer& other) {
    // Both AI players push away from each other equally
    Vec3 toOther = other.m_position - m_position;
    toOther.y = 0;
    f32 dist = glm::length(toOther);
    if (dist < 0.7f && dist > 0.01f) {
        Vec3 pushDir = glm::normalize(toOther);
        m_position -= pushDir * (0.7f - dist) * 0.5f;
        other.m_position += pushDir * (0.7f - dist) * 0.5f;
    }
}

// AIManager implementation

void AIManager::createTeams(f32 fieldLength) {
    m_players.clear();

    // Red Team (team 0) - attacks positive X goal
    AIPlayer gk1;
    gk1.setHomePosition(Vec3(-45.0f, 0.0f, 0.0f));
    gk1.setTeam(0);
    m_players.push_back(gk1);

    // Defenders positioned wide
    for (int i = 0; i < 2; i++) {
        AIPlayer def;
        def.setHomePosition(Vec3(-35.0f, 0.0f, -12.0f + i * 24.0f));
        def.setTeam(0);
        m_players.push_back(def);
    }

    // Midfielders
    for (int i = 0; i < 2; i++) {
        AIPlayer mid;
        mid.setHomePosition(Vec3(-15.0f, 0.0f, -15.0f + i * 30.0f));
        mid.setTeam(0);
        m_players.push_back(mid);
    }

    // Forward
    AIPlayer fwd;
    fwd.setHomePosition(Vec3(-5.0f, 0.0f, 0.0f));
    fwd.setTeam(0);
    m_players.push_back(fwd);

    // Blue Team (team 1) - attacks negative X goal (no forward, human player fills that role)
    AIPlayer gk2;
    gk2.setHomePosition(Vec3(45.0f, 0.0f, 0.0f));
    gk2.setTeam(1);
    m_players.push_back(gk2);

    for (int i = 0; i < 2; i++) {
        AIPlayer def;
        def.setHomePosition(Vec3(35.0f, 0.0f, -12.0f + i * 24.0f));
        def.setTeam(1);
        m_players.push_back(def);
    }

    for (int i = 0; i < 2; i++) {
        AIPlayer mid;
        mid.setHomePosition(Vec3(15.0f, 0.0f, -15.0f + i * 30.0f));
        mid.setTeam(1);
        m_players.push_back(mid);
    }
}

void AIManager::update(f32 deltaTime, Ball& ball, const Vec3& playerPos,
                       f32 fieldLength, f32 fieldWidth, f32 goalWidth) {
    // Determine which player on each team should chase
    findClosestChasers(ball.getPosition());

    for (auto& ai : m_players) {
        ai.update(deltaTime, ball, playerPos, fieldLength, fieldWidth, goalWidth);
    }

    handleCollisions(ball, playerPos);
}

void AIManager::findClosestChasers(const Vec3& ballPos) {
    // Find closest non-goalkeeper on each team to assign chase duty
    i32 closestRedIdx = -1;
    i32 closestBlueIdx = -1;
    f32 closestRedDist = 999.0f;
    f32 closestBlueDist = 999.0f;

    for (size_t i = 0; i < m_players.size(); i++) {
        f32 dist = m_players[i].distanceToBall(ballPos);
        bool isGoalkeeper = (i == 0) || (i == 6);  // First player of each team

        if (m_players[i].getTeam() == 0 && !isGoalkeeper) {
            if (dist < closestRedDist) {
                closestRedDist = dist;
                closestRedIdx = static_cast<i32>(i);
            }
        } else if (m_players[i].getTeam() == 1 && !isGoalkeeper) {
            if (dist < closestBlueDist) {
                closestBlueDist = dist;
                closestBlueIdx = static_cast<i32>(i);
            }
        }
    }

    // Update chase flags
    for (size_t i = 0; i < m_players.size(); i++) {
        bool isClosest = (static_cast<i32>(i) == closestRedIdx) ||
                         (static_cast<i32>(i) == closestBlueIdx);
        m_players[i].setIsClosestChaser(isClosest);
    }
}

void AIManager::handleCollisions(Ball& ball, const Vec3& playerPos) {
    // AI-ball and AI-human collisions
    for (auto& ai : m_players) {
        ai.handleBallCollision(ball);
        ai.handlePlayerCollision(playerPos);
    }

    // AI-AI collisions (O(n^2) but n is small)
    for (size_t i = 0; i < m_players.size(); i++) {
        for (size_t j = i + 1; j < m_players.size(); j++) {
            m_players[i].handleAICollision(m_players[j]);
        }
    }
}

}
