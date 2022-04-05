/*
Platform - A C++ framework
*/

#pragma once

#include <chrono>
#include <string>

namespace platform::core::timer {
/// Interface for timers
class ITimer {
public:
    virtual ~ITimer() = default;
    /**
     * @brief Start timer with set interval
     *
     */
    virtual void start() = 0;
    /**
     * @brief Start timer with specific interval
     *
     * @param interval
     */
    virtual void start(std::chrono::milliseconds interval) = 0;
    /**
     * @brief Stop timer
     *
     */
    virtual void stop() = 0;
    /**
     * @brief Reset timer
     *
     */
    virtual void reset() = 0;
    /**
     * @brief Get the id object
     *
     * @return timer Id
     */
    [[nodiscard]] virtual int getId() const = 0;
};
} // namespace platform::core::timer
