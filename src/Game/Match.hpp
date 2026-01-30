// Match.hpp
// Game state management: scoring, goal detection, and field boundaries.
#pragma once

#include "Core/Types.hpp"
#include "Ball.hpp"

namespace Sports {

class Match {
public:
    static constexpr f32 GOAL_CELEBRATION_DURATION = 3.0f;  // Pause after goal

    Match() = default;

    void setFieldDimensions(f32 fieldLength, f32 fieldWidth, f32 goalWidth, f32 goalHeight);
    void update(f32 deltaTime, Ball& ball);
    void reset();

    // Goal detection
    bool checkGoal(const Vec3& ballPos);
    void resetAfterGoal(Ball& ball);

    // Getters
    i32 getScoreLeft() const { return m_scoreLeft; }
    i32 getScoreRight() const { return m_scoreRight; }
    bool isGoalScored() const { return m_goalScored; }
    f32 getCelebrationTimer() const { return m_celebrationTimer; }
    f32 getCelebrationAlpha() const;
    i32 getLastScoringTeam() const { return m_lastScoringTeam; }

    // Ball out of bounds check
    bool isBallOutOfBounds(const Vec3& ballPos) const;
    void handleBoundaryCollision(Ball& ball);

private:
    f32 m_fieldLength = 105.0f;
    f32 m_fieldWidth = 68.0f;
    f32 m_goalWidth = 7.32f;
    f32 m_goalHeight = 2.44f;

    i32 m_scoreLeft = 0;   // Blue team (negative X goal)
    i32 m_scoreRight = 0;  // Red team (positive X goal)

    bool m_goalScored = false;
    f32 m_celebrationTimer = 0.0f;
    i32 m_lastScoringTeam = -1;  // 0 = red scored, 1 = blue scored
};

}
