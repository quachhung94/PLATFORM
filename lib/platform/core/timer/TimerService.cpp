/*
Platform - A C++ framework
*/

#include "platform/core/timer/TimerService.h"
#include "platform/config_constants.h"
#include "platform/core/task_priorities.h"
#include "platform/core/timer/Timer.h"

using namespace std::chrono;

namespace platform::core::timer {
TimerService::TimerService()
    : Task("TimerService", CONFIG_PLATFORM_TIMER_SERVICE_STACK_SIZE, TIMER_SERVICE_PRIO, milliseconds(0)),
      cmp([](const SharedTimer& left, const SharedTimer& right) { return left->expireAt() > right->expireAt(); }),
      m_queue(cmp),
      m_guard()
{
}

TimerService& TimerService::get()
{
    static TimerService service;

    return service;
}

void TimerService::startService() { get().start(); }

void TimerService::addTimer(const SharedTimer& timer)
{
    std::lock_guard<std::mutex> lock(m_guard);
    timer->calculate_next_execution();
    m_queue.push(timer);
    m_cond.notify_one();
}

void TimerService::removeTimer(const SharedTimer& timer)
{
    std::lock_guard<std::mutex> lock(m_guard);
    m_queue.removeTimer(timer);
    m_cond.notify_one();
}

void TimerService::tick()
{
    std::unique_lock<std::mutex> lock(m_guard);

    if (m_queue.empty()) {
        // No timers, wait until one is added.
        m_cond.wait_for(lock, seconds(1));
    }
    else {
        auto now = steady_clock::now();

        std::vector<SharedTimer> processed{};

        // Process any expired timers
        while (!m_queue.empty() && now >= m_queue.top()->expireAt()) {
            auto timer = m_queue.top();
            timer->expired();

            // Timer expired, remove from m_queue
            m_queue.pop();

            // If the timer is set "repeating", push it back the process queue.
            if (timer->isRepeating()) {
                processed.push_back(timer);
            }
        }

        for (auto& t : processed) {
            t->calculate_next_execution();
            m_queue.push(t);
        }

        if (!m_queue.empty()) {
            // Get next timer to expire
            auto timer = m_queue.top();

            // Wait for the timer to expire, or a timer to be removed or added.
            auto current_queue_length = m_queue.size();

            m_cond.wait_until(lock, timer->expireAt(), [current_queue_length, this]() {
                // Wake up if a timer has been added or removed.
                return current_queue_length != m_queue.size();
            });
        }
    }
}
} // namespace platform::core::timer
