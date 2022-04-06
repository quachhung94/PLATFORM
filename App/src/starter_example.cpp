/*
Platform - A C++ framework
*/

#include "starter_example.h"
#include "platform/core/SystemStatistics.h"
#include "spdlog/spdlog.h"

namespace starter_example {

App::App() : Application(platform::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1800)),
            m_timerEventQueue(ExpiredQueue::create(30, *this, *this)),
            timers()
{

}

void App::init()
{
    SPDLOG_INFO("App::Init Starting...");
    // platform::core::SystemStatistics::instance().dump();
    // a_instance.start();
    // this->m_wsMonitorTimeout = 10;
    this->a_instance = std::make_shared<ATask>();
    this->a_instance->start();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    create_timer(RB_FSM_WS_MONITOR_TIMER_ID, std::chrono::milliseconds(1000));
    create_timer(2, std::chrono::milliseconds(10000));
    create_timer(3, std::chrono::milliseconds(100000));
    create_timer(4, std::chrono::milliseconds(1000000));
    // this->create_timer(std::chrono::milliseconds(10000));
    // for (auto& t : this->timers)
    // {
    //     t.timer->start();
    // }
    // timers.front().timer->stop();
    // timers.front().timer->start();
    timers.at(1).timer->stop();
    timers.at(1).timer->start();

}

void App::tick()
{
    SPDLOG_INFO("App Hello world!");
    // SystemStatistics::instance().dump();
}

void App::event(const platform::core::timer::TimerExpiredEvent& event)
{
    auto& info = timers[static_cast<decltype(timers)::size_type>(event.getId())];
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - info.last);
    info.last = std::chrono::steady_clock::now();
    info.count++;
    info.total += duration;

    SPDLOG_INFO("Timer ID {} ({}ms): {}ms, avg: {}",
                    event.getId(),
                    info.interval.count(),
                    duration.count(),
                    static_cast<double>(info.total.count()) / info.count);
}

void App::create_timer(int id, std::chrono::milliseconds interval)
{
    TimerInfo t;
    t.timer = platform::core::timer::Timer::create(static_cast<int32_t>(id), m_timerEventQueue, true, interval);
    t.interval = interval;
    timers.push_back(t);
}

} // namespace starter_example
