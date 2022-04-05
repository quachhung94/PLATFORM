/*
Platform - A C++ framework
*/

#include "platform/core/timer/ElapsedTime.h"

using namespace std::chrono;

namespace platform::core::timer {
std::chrono::microseconds ElapsedTime::getRunningTime()
{
    if (active) {
        m_endTime = std::chrono::steady_clock::now();
        m_elapsed = m_endTime - m_startTime;
    }

    return duration_cast<microseconds>(m_elapsed);
}

std::chrono::microseconds ElapsedTime::getRunningTime() const
{
    steady_clock::duration local_elapsed{};

    if (active) {
        local_elapsed = steady_clock::now() - m_startTime;
    }

    return duration_cast<microseconds>(local_elapsed);
}
} // namespace platform::core::timer
