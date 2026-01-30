// Match.cpp
// Goal detection, score tracking, and ball boundary handling.
#include "Match.hpp"
#include "Core/Logger.hpp"
#include <cmath>

namespace Sports {

void Match::setFieldDimensions(f32 fieldLength, f32 fieldWidth, f32 goalWidth, f32 goalHeight) {
    m_fieldLength = fieldLength;
    m_fieldWidth = fieldWidth;
    m_goalWidth = goalWidth;
    m_goalHeight = goalHeight;
}

void Match::update(f32 deltaTime, Ball& ball) {
    if (m_goalScored) {
        m_celebrationTimer -= deltaTime;
        if (m_celebrationTimer <= 0.0f) {
            resetAfterGoal(ball);
        }
    } else {
        checkGoal(ball.getPosition());
    }
}

void Match::reset() {
    m_scoreLeft = 0;
    m_scoreRight = 0;
    m_goalScored = false;
    m_celebrationTimer = 0.0f;
    m_lastScoringTeam = -1;
}

bool Match::checkGoal(const Vec3& ballPos) {
    if (m_goalScored) {
        return false;
    }

    f32 goalLineX = m_fieldLength / 2.0f;
    f32 goalHalfWidth = m_goalWidth / 2.0f;
    f32 ballRadius = Ball::RADIUS;

    bool inGoalZ = std::abs(ballPos.z) < goalHalfWidth;
    bool inGoalY = ballPos.y < m_goalHeight && ballPos.y > 0;

    // Red team scores (ball in positive X goal)
    if (ballPos.x > goalLineX + ballRadius && inGoalZ && inGoalY) {
        m_scoreRight++;
        m_goalScored = true;
        m_celebrationTimer = GOAL_CELEBRATION_DURATION;
        m_lastScoringTeam = 0;
        LOG_INFO("GOAL! Red Team scores! Score: {} - {}", m_scoreLeft, m_scoreRight);
        return true;
    }
    // Blue team scores (ball in negative X goal)
    else if (ballPos.x < -goalLineX - ballRadius && inGoalZ && inGoalY) {
        m_scoreLeft++;
        m_goalScored = true;
        m_celebrationTimer = GOAL_CELEBRATION_DURATION;
        m_lastScoringTeam = 1;
        LOG_INFO("GOAL! Blue Team scores! Score: {} - {}", m_scoreLeft, m_scoreRight);
        return true;
    }

    return false;
}

void Match::resetAfterGoal(Ball& ball) {
    ball.reset();
    m_goalScored = false;
    m_celebrationTimer = 0.0f;
}

f32 Match::getCelebrationAlpha() const {
    if (!m_goalScored || m_celebrationTimer <= 0.0f) {
        return 0.0f;
    }

    f32 alpha = 1.0f;
    if (m_celebrationTimer > GOAL_CELEBRATION_DURATION - 0.5f) {
        // Fade in
        alpha = (GOAL_CELEBRATION_DURATION - m_celebrationTimer) * 2.0f;
    } else if (m_celebrationTimer < 1.0f) {
        // Fade out
        alpha = m_celebrationTimer;
    }
    return alpha;
}

bool Match::isBallOutOfBounds(const Vec3& ballPos) const {
    f32 halfLength = m_fieldLength / 2.0f;
    f32 halfWidth = m_fieldWidth / 2.0f;
    f32 goalHalfWidth = m_goalWidth / 2.0f;

    // Check if out on the sides (Z axis)
    if (std::abs(ballPos.z) > halfWidth) {
        return true;
    }

    // Check if out on the ends (X axis) but not in goal area
    if (std::abs(ballPos.x) > halfLength) {
        bool inGoalArea = std::abs(ballPos.z) < goalHalfWidth &&
                          ballPos.y < m_goalHeight;
        if (!inGoalArea) {
            return true;
        }
    }

    return false;
}

void Match::handleBoundaryCollision(Ball& ball) {
    Vec3 pos = ball.getPosition();
    Vec3 vel = ball.getVelocity();
    f32 halfLength = m_fieldLength / 2.0f;
    f32 halfWidth = m_fieldWidth / 2.0f;
    f32 goalHalfWidth = m_goalWidth / 2.0f;
    f32 radius = Ball::RADIUS;

    // Side boundaries (Z axis)
    if (pos.z < -halfWidth + radius) {
        ball.state().position.z = -halfWidth + radius;
        ball.state().velocity.z = std::abs(vel.z) * 0.6f;
    } else if (pos.z > halfWidth - radius) {
        ball.state().position.z = halfWidth - radius;
        ball.state().velocity.z = -std::abs(vel.z) * 0.6f;
    }

    // End boundaries (X axis) - only if not in goal area
    bool inGoalArea = std::abs(pos.z) < goalHalfWidth && pos.y < m_goalHeight;

    if (!inGoalArea) {
        if (pos.x < -halfLength + radius) {
            ball.state().position.x = -halfLength + radius;
            ball.state().velocity.x = std::abs(vel.x) * 0.6f;
        } else if (pos.x > halfLength - radius) {
            ball.state().position.x = halfLength - radius;
            ball.state().velocity.x = -std::abs(vel.x) * 0.6f;
        }
    }
}

}
