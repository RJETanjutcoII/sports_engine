// Timer.hpp
// High-resolution timing utilities for delta time and profiling.
#pragma once

#include "Types.hpp"
#include <chrono>

namespace Sports {

// Simple timer that measures elapsed time since construction or reset
class Timer {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::duration<f64>;

    Timer();

    void reset();
    f64 elapsed() const;       // Seconds since reset
    f64 elapsedMillis() const; // Milliseconds since reset
    f64 lap();                 // Returns elapsed and resets (for frame timing)

private:
    TimePoint m_startTime;
};

// Pausable timer for accumulating time across multiple intervals
class Stopwatch {
public:
    Stopwatch();

    void start();
    void stop();
    void reset();

    f64 elapsed() const;
    bool isRunning() const { return m_running; }

private:
    Timer::TimePoint m_startTime;
    f64 m_accumulated;
    bool m_running;
};

}
