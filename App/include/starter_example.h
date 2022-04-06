/*
Platform - A C++ framework
*/

#pragma once

#include "platform/core/Application.h"
#include "platform/core/task_priorities.h"
#include "platform/core/Task.h"
#include "platform/core/timer/Timer.h"
#include "platform/core/ipc/IEventListener.h"
#include "platform/core/ipc/Publisher.h"
#include "platform/core/ipc/SubscribingTaskEventQueue.h"
#include "spdlog/spdlog.h"
#include <iostream>

const int RB_FSM_WS_MONITOR_TIMER_ID = 100;
#define DEFAULT_WS_MONITOR_TIMEOUT        10
namespace starter_example {
class ATask : public platform::core::Task {
public:
    ATask() : platform::core::Task("Other task", 9000, platform::core::APPLICATION_BASE_PRIO, std::chrono::seconds{1}) {}

    void tick() override { SPDLOG_INFO("App::Init Hello from other task"); }
};

class App : public platform::core::Application,
                public platform::core::ipc::IEventListener<platform::core::timer::TimerExpiredEvent>
{
public:
    using ExpiredQueue = platform::core::ipc::TaskEventQueue<platform::core::timer::TimerExpiredEvent>;
    App();

    void init() override;

    void tick() override;

    std::shared_ptr<ATask> a_instance;

    void event(const platform::core::timer::TimerExpiredEvent& event) override;

private:
    void create_timer(int id, std::chrono::milliseconds interval);
    struct TimerInfo
    {
        platform::core::timer::TimerOwner timer;
        std::chrono::milliseconds interval;
        std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();
        int count = 0;
        std::chrono::milliseconds total = std::chrono::milliseconds(0);
    };
    std::shared_ptr<ExpiredQueue> m_timerEventQueue;
    std::vector<TimerInfo> timers;
};
} // namespace starter_example
