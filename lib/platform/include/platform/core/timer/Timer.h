/*
Platform - A C++ framework
*/

#pragma once

#include "platform/core/ipc/TaskEventQueue.h"
#include "platform/core/timer/Timer.h"
#include "platform/core/timer/TimerExpiredEvent.h"
#include <chrono>
#include <functional>
#include <string>

namespace platform::core::timer {
class TimerService;

class Timer;

/// RAII helper for Timer.
class TimerOwner {
public:
    TimerOwner(int id, const std::weak_ptr<ipc::TaskEventQueue<timer::TimerExpiredEvent>>& event_queue,
               bool auto_reload, std::chrono::milliseconds interval);

    TimerOwner() = default;

    TimerOwner(const TimerOwner&) = default;

    TimerOwner& operator=(const TimerOwner&) = default;

    TimerOwner(TimerOwner&&) = default;

    TimerOwner& operator=(TimerOwner&&) = default;

    ~TimerOwner();

    std::shared_ptr<Timer> operator->() const noexcept;

    explicit operator bool() const noexcept { return static_cast<bool>(t); }

private:
    friend Timer;
    explicit TimerOwner(std::shared_ptr<Timer> t) noexcept;

    std::shared_ptr<Timer> t;
};

/**
 * @brief Timer Object which attaches to an event queue, trigger event when expires
 *
 */
class Timer : public ITimer, public std::enable_shared_from_this<Timer> {
public:
    /**
     * @brief Create method for TimerOwner
     *
     * @param id The ID of the timer
     * @param event_queue The event queue attach to
     * @param auto_reload If true, timer restart automatically when it expires
     * @param interval The interval of timer
     * @return TimerOwner
     */
    static TimerOwner create(int id, const std::weak_ptr<ipc::TaskEventQueue<timer::TimerExpiredEvent>>& event_queue,
                             bool auto_reload, std::chrono::milliseconds interval);

    ~Timer() override = default;

    /**
     * @brief Start the timer
     *
     */
    void start() override;

    /**
     * @brief Start the timer with specific interval
     *
     * @param interval
     */
    void start(std::chrono::milliseconds interval) override;

    /**
     * @brief Stop the timer
     *
     */
    void stop() override;

    /**
     * @brief Reset the timer
     *
     */
    void reset() override;

    /**
     * @brief Get the id of timer
     *
     * @return int
     */
    int getId() const override;

    /**
     * @brief Get timer repeating status
     *
     * @return true if timer is repeating
     * @return false if timer is not repeating
     */
    bool isRepeating() const { return m_repeating; }

    /**
     * @brief Get the time point when timer is expired
     *
     * @return std::chrono::steady_clock::time_point
     */
    std::chrono::steady_clock::time_point expireAt() const;

protected:
    int m_id;
    bool m_repeating;
    std::chrono::milliseconds m_timerInterval;
    /**
     * @brief Construct a new Timer object
     *
     * @param id The ID of the timer.
     * @param event_queue The event queue to send events on.
     * @param auto_reload If true, timer restart automatically when it expires
     * @param interval The interval of timer
     */
    Timer(int id, std::weak_ptr<ipc::TaskEventQueue<timer::TimerExpiredEvent>> event_queue, bool auto_reload,
          std::chrono::milliseconds interval);

private:
    friend class platform::core::timer::TimerService;

    void expired();

    void calculate_next_execution();

    std::weak_ptr<ipc::TaskEventQueue<TimerExpiredEvent>> m_queue;
    std::chrono::steady_clock::time_point m_expireTime;
};
} // namespace platform::core::timer
