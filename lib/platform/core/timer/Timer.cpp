/*
Platform - A C++ framework
*/

#include "platform/core/timer/Timer.h"
#include "platform/core/timer/TimerService.h"
#include "platform/core/util/create_protected.h"

using namespace platform::core::util;
using namespace std::chrono;

namespace platform::core::timer {
Timer::Timer(int id, std::weak_ptr<ipc::TaskEventQueue<TimerExpiredEvent>> event_queue, bool repeating,
             milliseconds interval)
    : m_id(id),
      m_repeating(repeating),
      m_timerInterval(interval),
      m_queue(std::move(event_queue)),
      m_expireTime(steady_clock::now())
{
    TimerService::startService();
}

void Timer::start()
{
    stop();
    TimerService::get().addTimer(shared_from_this());
}

void Timer::start(milliseconds interval)
{
    this->m_timerInterval = interval;
    start();
}

void Timer::stop() { TimerService::get().removeTimer(shared_from_this()); }

void Timer::reset()
{
    stop();
    start();
}

int Timer::getId() const { return m_id; }

void Timer::expired()
{
    TimerExpiredEvent ev(m_id);
    const auto& q = m_queue.lock();

    if (q) {
        q->push(ev);
    }
}

TimerOwner Timer::create(int id, const std::weak_ptr<ipc::TaskEventQueue<timer::TimerExpiredEvent>>& event_queue,
                         bool auto_reload, std::chrono::milliseconds interval)
{
    return TimerOwner(create_protected_shared<Timer>(id, event_queue, auto_reload, interval));
}

std::chrono::steady_clock::time_point Timer::expireAt() const { return m_expireTime; }

void Timer::calculate_next_execution() { m_expireTime = steady_clock::now() + m_timerInterval; }

TimerOwner::TimerOwner(std::shared_ptr<Timer> t) noexcept : t(std::move(t)) {}

TimerOwner::TimerOwner(int id, const std::weak_ptr<ipc::TaskEventQueue<timer::TimerExpiredEvent>>& event_queue,
                       bool auto_reload, std::chrono::milliseconds interval)
{
    *this = Timer::create(id, event_queue, auto_reload, interval);
}

TimerOwner::~TimerOwner()
{
    if (t) {
        t->stop();
    }
}

std::shared_ptr<Timer> TimerOwner::operator->() const noexcept { return t; }
} // namespace platform::core::timer
