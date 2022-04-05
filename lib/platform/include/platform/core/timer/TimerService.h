/*
Platform - A C++ framework
*/

#pragma once

#include "platform/core/Task.h"
#include <algorithm>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>

namespace platform::core::timer {
class Timer;

using SharedTimer = std::shared_ptr<Timer>;

using TimerComparator = std::function<bool(SharedTimer left, SharedTimer right)>;

/**
 * @brief TimerQueue extend the priority_queue by adding removeTimer method
 *
 */
class TimerQueue : public std::priority_queue<SharedTimer, std::vector<SharedTimer>, TimerComparator> {
public:
    explicit TimerQueue(const TimerComparator& cmp)
        : std::priority_queue<SharedTimer, std::vector<SharedTimer>, TimerComparator>(cmp)
    {
    }

    void removeTimer(const SharedTimer& timer)
    {
        auto it = find_if(c.begin(), c.end(), [timer](const SharedTimer& o) {
            // Compare pointers
            bool found = timer == o;

            return found;
        });

        if (it != c.end()) {
            c.erase(it);
        }
    }
};

/**
 * @brief Container in which stores Timer instances. Running a loop check, which attach to a Task. Signal an event to
 * the event queue of corresponding timer
 *
 */
class TimerService : private platform::core::Task {
public:
    TimerService();

    static void startService();

    static TimerService& get();

    void addTimer(const SharedTimer& timer);

    void removeTimer(const SharedTimer& timer);

protected:
    void tick() override;

private:
    TimerComparator cmp;
    TimerQueue m_queue;
    std::mutex m_guard;
    std::condition_variable m_cond{};
};
} // namespace platform::core::timer
