// Timer.cpp
// Chrono-based timing implementation.
#include "Timer.hpp"

namespace Sports {

Timer::Timer() {
    reset();
}

void Timer::reset() {
    m_startTime = Clock::now();
}

f64 Timer::elapsed() const {
    Duration elapsed = Clock::now() - m_startTime;
    return elapsed.count();
}

f64 Timer::elapsedMillis() const {
    return elapsed() * 1000.0;
}

f64 Timer::lap() {
    f64 time = elapsed();
    reset();
    return time;
}

Stopwatch::Stopwatch()
    : m_accumulated(0.0)
    , m_running(false) {
}

void Stopwatch::start() {
    if (!m_running) {
        m_startTime = Timer::Clock::now();
        m_running = true;
    }
}

void Stopwatch::stop() {
    if (m_running) {
        Timer::Duration elapsed = Timer::Clock::now() - m_startTime;
        m_accumulated += elapsed.count();
        m_running = false;
    }
}

void Stopwatch::reset() {
    m_accumulated = 0.0;
    m_running = false;
}

f64 Stopwatch::elapsed() const {
    if (m_running) {
        Timer::Duration currentSegment = Timer::Clock::now() - m_startTime;
        return m_accumulated + currentSegment.count();
    }
    return m_accumulated;
}

}
