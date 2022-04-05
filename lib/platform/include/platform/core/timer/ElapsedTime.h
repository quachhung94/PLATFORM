/*
Platform - A C++ framework
*/

#pragma once

#include <chrono>

namespace platform::core::timer {
/**
 * @brief Use to measure time between two point of time
 *
 */
class ElapsedTime {
public:
    /**
     * @brief Start Elapsed Timer
     *
     */
    void start()
    {
        active = true;
        zero();
    }

    /**
     * @brief Stop Elapsed Timer
     *
     */
    void stop()
    {
        m_endTime = std::chrono::steady_clock::now();
        active    = false;
        m_elapsed = m_endTime - m_startTime;
    }

    /**
     * @brief Reset Elapsed Timer
     *
     */
    void reset()
    {
        // Simply restart the timer.
        start();
    }

    /**
     * @brief Zero timer but keep it running
     *
     */
    void zero()
    {
        m_startTime = std::chrono::steady_clock::now();
        m_endTime   = m_startTime;
    }

    /**
     * @brief Stop Elapsed Timer and zero it
     *
     */
    void stop_and_zero()
    {
        stop();
        zero();
    }

    /**
     * @brief Get the running time
     *
     * @return std::chrono::microseconds
     */
    std::chrono::microseconds getRunningTime();

    [[nodiscard]] std::chrono::microseconds getRunningTime() const;

    /**
     * @brief Get the status of Elapsed Timer
     *
     * @return true if timer is running
     * @return false if timer is not running
     */
    [[nodiscard]] bool is_running() const { return active; }

private:
    bool active = false;
    std::chrono::steady_clock::time_point m_startTime{};
    std::chrono::steady_clock::time_point m_endTime{};
    std::chrono::steady_clock::duration m_elapsed{};
};
} // namespace platform::core::timer
