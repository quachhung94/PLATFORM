/*
Platform - A C++ framework
*/

#pragma once

#include "platform/core/timer/ITimer.h"

namespace platform::core::timer {
class TimerExpiredEvent {
public:
    TimerExpiredEvent() = default;

    explicit TimerExpiredEvent(int id) : id(id) {}

    /**
     * @brief Get the Id object
     * 
     * @return int 
     */
    [[nodiscard]] int getId() const { return id; }

private:
    int id = -1;
};
} // namespace platform::core::timer
